#pragma once
#include <vulkan/vulkan.hpp>
#include "Core/Common.h"
#include "Graphics/GraphicsCore.h"

namespace Snow {
    class VkCore : public Graphics {
    public:
        VkCore();
        virtual ~VkCore() override;

    private:
        std::vector<const char*> GetInstanceExtensions();
        void CreateInstance();

        vk::Instance mInstance;

        static VkCore* sInstance;
    };
}