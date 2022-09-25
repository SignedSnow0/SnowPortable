#include "Surface.h"
#include "Graphics/GraphicsCore.h"
#include "Platform/Vulkan/VkSurface.h"
#include "Core/Logger.h"

namespace Snow {
    Surface* Surface::Create(const Window& window) {
        switch (Graphics::Api()) {
            case GraphicsAPI::Vulkan: return new VkSurface(window);
        }

        LOG_ERROR("Tried to create surface with invalid graphics API");
        return nullptr;
    }
}