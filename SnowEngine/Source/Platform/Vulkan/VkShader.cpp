#include "VkShader.h"
#include <shaderc/shaderc.hpp>
#include "Core/Utils.h"
#include "Core/Logger.h"
#include "VkCore.h"

#ifdef SNOW_DEBUG
#pragma comment(lib, "shaderc_combinedd.lib")
#else
#pragma comment(lib, "shaderc_combined.lib")
#endif

namespace Snow {
    static shaderc_shader_kind GetShadercShaderKind(vk::ShaderStageFlagBits stage) {
        switch (stage) {
        case vk::ShaderStageFlagBits::eVertex: return shaderc_glsl_vertex_shader;
        case vk::ShaderStageFlagBits::eFragment: return shaderc_glsl_fragment_shader;
        case vk::ShaderStageFlagBits::eCompute: return shaderc_glsl_compute_shader;
        default: LOG_ERROR("Compiling shader with unsupported stage"); return shaderc_glsl_vertex_shader;
        }
    }

    static vk::ShaderStageFlagBits GetVkShaderStage(ShaderStage stage) {
        switch (stage) {
        case ShaderStage::Vertex: return vk::ShaderStageFlagBits::eVertex;
        case ShaderStage::Fragment: return vk::ShaderStageFlagBits::eFragment;
        case ShaderStage::Compute: return vk::ShaderStageFlagBits::eCompute;
        default: LOG_ERROR("Compiling shader with unsupported stage"); return vk::ShaderStageFlagBits::eVertex;
        }
    }

    VkShader::VkShader(const std::vector<std::pair<std::filesystem::path, ShaderStage>>& shaders) {
        for (const auto& [file, stage] : shaders) {
            vk::ShaderStageFlagBits vkStage{ GetVkShaderStage(stage) };
            
            if (mShaderModules.find(vkStage) != mShaderModules.end()) {
                LOG_ERROR("Compiling shader with duplicate stage");
                continue;
            }

            mShaderModules.insert({ vkStage, CompileShader(file, vkStage) });
        }
    }

    VkShader::~VkShader() {
        for (const auto& [stage, module] : mShaderModules) {
            VkCore::Instance()->Device().destroyShaderModule(module);
        }
    }

    std::vector<vk::PipelineShaderStageCreateInfo> VkShader::ShaderStages() const {
        std::vector<vk::PipelineShaderStageCreateInfo> stages{};
        for (const auto& [stage, module] : mShaderModules) {
            stages.push_back({ {}, stage, module, "main" });
        }

        return stages;
    }

    vk::ShaderModule VkShader::CompileShader(const std::filesystem::path& file, vk::ShaderStageFlagBits stage) {
        static shaderc::Compiler compiler{};
        shaderc::CompileOptions options{};
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);

        auto data{ Utils::ReadFile(file) };
        if (data.empty()) {
            return {};
        }

        std::string code{ reinterpret_cast<char*>(data.data()), data.size() };
        shaderc::SpvCompilationResult result{ compiler.CompileGlslToSpv(code, GetShadercShaderKind(stage), file.filename().string().c_str()) };

        if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
            LOG_ERROR("Failed to compile shader: {}", result.GetErrorMessage());
            LOG_ERROR("Shader file: {}", file.string());
            return {};
        }

        std::vector<u32> spirv{ result.begin(), result.end() };

        vk::ShaderModuleCreateInfo createInfo{};
        createInfo.codeSize = static_cast<u32>(spirv.size()) * sizeof(u32);
        createInfo.pCode = spirv.data();

        return VkCore::Instance()->Device().createShaderModule(createInfo);
    }
}