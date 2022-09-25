#pragma once
#include <vulkan/vulkan.hpp>
#include "Core/Common.h"
#include "Graphics/GraphicsCore.h"

namespace Snow {
    struct QueueFamilies {
        std::pair<uint32_t, vk::Queue> Graphics{ UINT32_MAX, nullptr };
        std::pair<uint32_t, vk::Queue> Present{ UINT32_MAX, nullptr };

        QueueFamilies() = default;
        QueueFamilies(vk::Instance instance, vk::PhysicalDevice device);
        
        b8 IsComplete() const;
    };
    
    class VkCore : public Graphics {
    public:
        VkCore();
        virtual ~VkCore() override;

        vk::Instance VkInstance() const;
        vk::PhysicalDevice PhysicalDevice() const;
        vk::Device Device() const;
        const QueueFamilies& QueueFamilyIndices() const;

        static const VkCore* Instance();

    private:
        std::vector<const char*> GetInstanceLayers();
        std::vector<const char*> GetInstanceExtensions();
        std::vector<const char*> GetDeviceExtensions();
        VkDebugUtilsMessengerCreateInfoEXT GetDebugMessengerCreateInfo();
        void CreateInstance();
        b8 ValidatePhysicalDevice(vk::PhysicalDevice device, QueueFamilies& outFamilies);
        void CreatePhysicalDevice();
        void CreateDevice();

        vk::Instance mInstance;
        b8 mValidationEnabled{ false };
        vk::DebugUtilsMessengerEXT mDebugMessenger;
        QueueFamilies mQueues;
        vk::PhysicalDevice mPhysicalDevice;
        vk::Device mDevice;
        
        static VkCore* sInstance;
    };
}