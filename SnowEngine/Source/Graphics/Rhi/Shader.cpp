#include "Shader.h"
#include "Graphics/GraphicsCore.h"
#include "Platform/Vulkan/VkShader.h"
#include "Core/Logger.h"

namespace Snow {
    Shader* Shader::Create(const std::vector<std::pair<std::filesystem::path, ShaderStage>>& shaders, const std::string& name) {
        switch (Graphics::Api()) {
        case GraphicsAPI::Vulkan:
            VkShader* shader = new VkShader(shaders);
            shader->mName = name;
            return shader;
        }

        LOG_ERROR("Tried to create shader with invalid graphics API");
        return nullptr;
    }

    const std::string& Shader::Name() const { return mName; }
}