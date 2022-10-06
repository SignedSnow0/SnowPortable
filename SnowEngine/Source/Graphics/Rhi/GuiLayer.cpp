#include "GuiLayer.h"
#include "Core/Logger.h"
#include "Graphics/GraphicsCore.h"
#include "Platform/Vulkan/VkGuiLayer.h"
#include <imgui.h>

namespace Snow {
    static ImVec4 RGBAtoVec4(const vec4u& color) {
        return ImVec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
    }

    static ImVec4 RGBtoVec4(const vec3u& color) {
        return ImVec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, 1.0f);
    }

    GuiLayer* GuiLayer::Create(const RenderTarget& rt, RenderPass* scene) {
        switch (Graphics::Api()) {
        case GraphicsAPI::Vulkan: return new VkGuiLayer(rt, scene);
        }
        
        LOG_ERROR("Tried to create gui layer with invalid graphics API");
        return nullptr;
    }

    void GuiLayer::DarkTheme() const {
    
    }
}