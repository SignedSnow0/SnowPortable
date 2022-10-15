#define VMA_IMPLEMENTATION
#include "VkCore.h"
#include <set>
#include <GLFW/glfw3.h>
#include "Core/Logger.h"
#include "Platform/Window.h"

namespace Snow {
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        switch (messageSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            LOG_TRACE(pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            LOG_INFO(pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LOG_WARN(pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LOG_ERROR(pCallbackData->pMessage);
            break;
        }
        return VK_FALSE;
    }

    static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        return func ? func(instance, pCreateInfo, pAllocator, pDebugMessenger) : VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    QueueFamilies::QueueFamilies(vk::Instance instance, vk::PhysicalDevice device) {
        std::vector<vk::QueueFamilyProperties> families{ device.getQueueFamilyProperties() };
        Window window{ "Test", 800, 600, false };
        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface(instance, reinterpret_cast<GLFWwindow*>(window.Native()), nullptr, &surface) != VK_SUCCESS) {
            LOG_ASSERT(false, "Failed to create window surface for queue family detection");
        }
        
        u32 i{ 0 };
        for (auto& family : families) {
            if (family.queueFlags & vk::QueueFlagBits::eGraphics) {
                Graphics.first = i;
            }

            if (device.getSurfaceSupportKHR(i, surface)) {
                Present.first = i;
            }

            if (IsComplete())
                break;
        }

        vkDestroySurfaceKHR(instance, surface, nullptr);
    }

    b8 QueueFamilies::IsComplete() const {
        return Graphics.first != UINT32_MAX && Present.first != UINT32_MAX;
    }

    VkCore* VkCore::sInstance{ nullptr };

    VkCore::VkCore() {
        if (sInstance) {
            return;
        }

        CreateInstance();
        CreatePhysicalDevice();
        CreateDevice();
        CreateAllocator();
        CreateCommandPool();

        sInstance = this;
    }

    VkCore::~VkCore() {
        if (sInstance != this) {
            return;
        }

        mDevice.destroyCommandPool(mCommandPool);

        vmaDestroyAllocator(mAllocator);
        
        mDevice.destroy();
        
        if (mValidationEnabled)
            DestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);

        mInstance.destroy();

        sInstance = nullptr;
    }

    vk::Instance VkCore::VkInstance() const { return mInstance; }

    vk::PhysicalDevice VkCore::PhysicalDevice() const { return mPhysicalDevice; }

    vk::Device VkCore::Device() const { return mDevice; }

    const QueueFamilies& VkCore::QueueFamilyIndices() const { return mQueues; }
    
    void VkCore::SubmitInstantCommand(std::function<void(vk::CommandBuffer)>&& command) const {
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandPool = mCommandPool;
        allocInfo.commandBufferCount = 1;

        vk::CommandBuffer commandBuffer{ mDevice.allocateCommandBuffers(allocInfo)[0] };

        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

        commandBuffer.begin(beginInfo);

        command(commandBuffer);

        commandBuffer.end();

        vk::SubmitInfo submitInfo{};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        
        mQueues.Graphics.second.submit(submitInfo, nullptr);
        mQueues.Graphics.second.waitIdle();

        mDevice.resetCommandPool(mCommandPool);
    }

    const VkCore* VkCore::Instance() { return sInstance; }

    VmaAllocator VkCore::Allocator() const { return mAllocator; }
    
    std::vector<const char*> VkCore::GetInstanceLayers() {
        std::vector<const char*> layers{};

#ifdef SNOW_DEBUG
        const char* validationLayer{ "VK_LAYER_KHRONOS_validation" };
        auto allLayers = vk::enumerateInstanceLayerProperties();
        
        for (auto& layer : allLayers) {
            if (strcmp(validationLayer, layer.layerName) == 0) {
                layers.push_back(validationLayer);
                mValidationEnabled = true;
                break;
            }
        }
#endif

        //TODO: Add instance layers here

        return layers;
    }

    std::vector<const char*> VkCore::GetInstanceExtensions() {
        u32 glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions{ glfwExtensions, glfwExtensions + glfwExtensionCount };
        if (mValidationEnabled) { //set in GetInstanceLayers()
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        //TODO: Add instance extensions here

        return extensions;
    }

    std::vector<const char*> VkCore::GetDeviceExtensions() {
        std::vector<const char*> extensions{};
        extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        //TODO: Add device extensions here

        return extensions;
    }


    VkDebugUtilsMessengerCreateInfoEXT VkCore::GetDebugMessengerCreateInfo() {
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
        createInfo.pUserData = nullptr;

        return createInfo;
    }
    
    void VkCore::CreateInstance() {
        vk::ApplicationInfo appInfo;
        appInfo.pApplicationName = "SnowEditor";
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
        appInfo.pEngineName = "SnowEngine";
        appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
        appInfo.apiVersion = VK_API_VERSION_1_2;

        auto layers = GetInstanceLayers();
        auto extensions = GetInstanceExtensions();
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = GetDebugMessengerCreateInfo();
        
        vk::InstanceCreateInfo createInfo;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<u32>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledLayerCount = static_cast<u32>(layers.size());
        createInfo.ppEnabledLayerNames = layers.data();
        createInfo.pNext = mValidationEnabled ? &debugCreateInfo : nullptr;

        mInstance = vk::createInstance(createInfo);

        VkDebugUtilsMessengerEXT debugMessenger;
        CreateDebugUtilsMessengerEXT(mInstance, &debugCreateInfo, nullptr, &debugMessenger);
        mDebugMessenger = debugMessenger;
    }

    b8 VkCore::ValidatePhysicalDevice(vk::PhysicalDevice device, QueueFamilies& outFamilies) {
        vk::PhysicalDeviceProperties properties{ device.getProperties() };
        vk::PhysicalDeviceFeatures features{ device.getFeatures() };
        QueueFamilies families{ mInstance, device };
        std::vector<vk::ExtensionProperties> extensions{ device.enumerateDeviceExtensionProperties() };

        auto requiredExtensions = GetDeviceExtensions();
        u32 remainingExtensions{ static_cast<u32>(requiredExtensions.size()) };
        for (auto& extension : extensions) {
            for (auto& requiredExtension : requiredExtensions) {
                if (strcmp(extension.extensionName, requiredExtension) == 0) {
                    --remainingExtensions;
                    if (remainingExtensions == 0)
                        break;
                }
            }
        }
        
        b8 valid{ true };
        valid &= properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu;
        valid &= families.IsComplete();
        valid &= remainingExtensions == 0;

        if (valid)
            outFamilies = families;
        return valid;
    }

    void VkCore::CreatePhysicalDevice() {
        std::vector<vk::PhysicalDevice> devices{ mInstance.enumeratePhysicalDevices() };

        for (auto& device : devices) {
            if (ValidatePhysicalDevice(device, mQueues)) {
                mPhysicalDevice = device;
                return;
            }
        }

        LOG_ASSERT(false, "No suitable physical device found");
    }

    void VkCore::CreateDevice() {
        f32 queuePriority{ 1.0f };
        std::vector<const char*> layers{ GetInstanceLayers() };
        vk::PhysicalDeviceFeatures features{ mPhysicalDevice.getFeatures() };
        
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos{};
        vk::DeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.queueFamilyIndex = mQueues.Graphics.first;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);

        if (mQueues.Graphics.first != mQueues.Present.first) {
            queueCreateInfo.queueFamilyIndex = mQueues.Present.first;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        std::vector<const char*> extensions{ GetDeviceExtensions() };

        vk::DeviceCreateInfo createInfo;
        createInfo.queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &features;
        createInfo.enabledExtensionCount = static_cast<u32>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledLayerCount = static_cast<u32>(layers.size());
        createInfo.ppEnabledLayerNames = layers.data();

        mDevice = mPhysicalDevice.createDevice(createInfo);

        mQueues.Graphics.second = mDevice.getQueue(mQueues.Graphics.first, 0);
        mQueues.Present.second = mDevice.getQueue(mQueues.Present.first, 0);
    }

    void VkCore::CreateAllocator() {
        VmaAllocatorCreateInfo createInfo{};
        createInfo.physicalDevice = mPhysicalDevice;
        createInfo.device = mDevice;
        createInfo.instance = mInstance;

        vmaCreateAllocator(&createInfo, &mAllocator);
    }

    void VkCore::CreateCommandPool() {
        vk::CommandPoolCreateInfo createInfo{};
        createInfo.queueFamilyIndex = mQueues.Graphics.first;
        createInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

        mCommandPool = mDevice.createCommandPool(createInfo);
    }
}