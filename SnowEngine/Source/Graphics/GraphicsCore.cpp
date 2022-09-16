#include "GraphicsCore.h"
#include "Core/Logger.h"

#include "Platform/Vulkan/VkCore.h"

namespace Snow {
    Graphics* Graphics::sInstance{ nullptr };

    b8 Graphics::Initialize(const GraphicsAPI& api) {
        if (sInstance) {
            LOG_WARN("Graphics system already initialized");
            return false;
        }

        switch (api) {
            case GraphicsAPI::Vulkan: {
                sInstance = new VkCore();
                LOG_INFO("Graphics system initialized using Vulkan");
                
                break;
            }
            default: {
                LOG_ERROR("Invalid graphics API");
                return false;
            }
        }
        
        return true;
    }

    void Graphics::Shutdown() {
        if (!sInstance) {
            return;
        }

        delete sInstance;
        sInstance = nullptr;
        LOG_INFO("Graphics system shutdown");
    }
}