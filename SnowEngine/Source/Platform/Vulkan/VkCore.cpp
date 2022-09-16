#include "VkCore.h"
#include <GLFW/glfw3.h>

namespace Snow {
    VkCore* VkCore::sInstance{ nullptr };

    VkCore::VkCore() {
        if (sInstance) {
            return;
        }

        CreateInstance();

        sInstance = this;
    }

    VkCore::~VkCore() {
        if (sInstance != this) {
            return;
        }

        mInstance.destroy();

        sInstance = nullptr;
    }

    std::vector<const char*> VkCore::GetInstanceExtensions() {
        u32 glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions{ glfwExtensions, glfwExtensions + glfwExtensionCount };
        //TODO: Add instance extensions here
        
        return extensions;
    }
    
    void VkCore::CreateInstance() {
        vk::ApplicationInfo appInfo;
        appInfo.pApplicationName = "SnowEditor";
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
        appInfo.pEngineName = "SnowEngine";
        appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
        appInfo.apiVersion = VK_API_VERSION_1_2;

        auto extensions = GetInstanceExtensions();
        
        vk::InstanceCreateInfo createInfo;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<u32>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;

        mInstance = vk::createInstance(createInfo);
    }
}