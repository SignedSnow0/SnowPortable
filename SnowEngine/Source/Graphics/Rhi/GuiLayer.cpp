#include "GuiLayer.h"
#include "Core/Logger.h"
#include "Graphics/GraphicsCore.h"
#include "Platform/Vulkan/VkGuiLayer.h"

namespace Snow {
    GuiLayer* GuiLayer::Create(const RenderTarget& rt) {
        switch (Graphics::Api()) {
            case GraphicsAPI::Vulkan: return new VkGuiLayer(rt);
        }

        LOG_ERROR("Tried to create gui layer with invalid graphics API");
        return nullptr;
    }
}