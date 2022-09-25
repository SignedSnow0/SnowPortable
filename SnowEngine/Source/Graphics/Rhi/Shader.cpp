#include "Shader.h"
#include "Graphics/GraphicsCore.h"
#include "Platform/Vulkan/VkShader.h"
#include "Core/Logger.h"

namespace Snow {
    Shader* Shader::Create(const std::vector<std::pair<std::filesystem::path, ShaderStage>>& shaders) {
        switch (Graphics::Api()) {
            case GraphicsAPI::Vulkan: return new VkShader(shaders);
        }

        LOG_ERROR("Tried to create shader with invalid graphics API");
        return nullptr;
    }
}