#pragma once
#include <map>
#include <vulkan/vulkan.hpp>
#include "Graphics/Rhi/Shader.h"

namespace Snow {
    class VkShader : public Shader {
    public:
        VkShader(const std::vector<std::pair<std::filesystem::path, ShaderStage>>& shaders);
        virtual ~VkShader() override;

        std::vector<vk::PipelineShaderStageCreateInfo> ShaderStages() const;

    private:
        vk::ShaderModule CompileShader(const std::filesystem::path& file, vk::ShaderStageFlagBits stage);

        std::map<vk::ShaderStageFlagBits, vk::ShaderModule> mShaderModules;
    };
}