#include "Pipeline.h"
#include "Core/Logger.h"
#include "Graphics/GraphicsCore.h"
#include "Platform/Vulkan/VkPipeline.h"

namespace Snow {
    Pipeline* Pipeline::Create(const PipelineCreateInfo& createInfo) {
        switch (Graphics::Api()) {
            case GraphicsAPI::Vulkan: return new VkPipeline(createInfo);
        }

        LOG_ERROR("Tried to create pipeline with invalid graphics API");
        return nullptr;
    }
}