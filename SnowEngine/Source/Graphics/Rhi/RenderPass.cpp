#include "RenderPass.h"
#include "Core/Logger.h"
#include "Graphics/GraphicsCore.h"
#include "Platform/Vulkan/VkRenderPass.h"

namespace Snow {
    RenderPass* RenderPass::Create(const RenderPassCreateInfo& createInfo) {
        switch (Graphics::Api()) {
            case GraphicsAPI::Vulkan: return new VkRenderPass(createInfo);
        }

        LOG_ERROR("Tried to create render pass with invalid graphics API");
        return nullptr;
    }
}