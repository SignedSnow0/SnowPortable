#include "VkShader.h"
#include <functional>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include "Core/Utils.h"
#include "Core/Logger.h"
#include "VkCore.h"


#ifdef SNOW_DEBUG
#pragma comment(lib, "shaderc_combinedd.lib")
#pragma comment(lib, "spirv-cross-cored.lib")
#else
#pragma comment(lib, "shaderc_combined.lib")
#pragma comment(lib, "spirv-cross-core.lib")
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

    VkShaderLayout::VkShaderLayout(const std::map<u32, ShaderResource*>& resources)
        : mResources{ resources } {
        std::vector<vk::DescriptorSetLayoutBinding> bindings{};
        bindings.resize(resources.size());
        
        u32 i{ 0 };
        for (const auto& [location, resource] : resources) {
            bindings[i++] = reinterpret_cast<VkShaderResource*>(resource)->Description;
        }

        vk::DescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.bindingCount = static_cast<u32>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        mLayout = VkCore::Instance()->Device().createDescriptorSetLayout(layoutInfo);
    }

    VkShaderLayout::~VkShaderLayout() {
        VkCore::Instance()->Device().destroyDescriptorSetLayout(mLayout);

        for (auto& [binding, resource] : mResources) {
            delete resource;
        }
    }

    const std::map<u32, ShaderResource*>& VkShaderLayout::Resources() const { return mResources; }
    
    vk::DescriptorSetLayout VkShaderLayout::Layout() const { return mLayout; }

    VkShader::VkShader(const std::vector<std::pair<std::filesystem::path, ShaderStage>>& shaders)
        : mSources{ shaders } {
        CreateShader();
    }

    VkShader::~VkShader() {
        DestroyShader();
    }

    std::vector<vk::PipelineShaderStageCreateInfo> VkShader::ShaderStages() const {
        std::vector<vk::PipelineShaderStageCreateInfo> stages{};
        for (const auto& [stage, module] : mShaderModules) {
            stages.push_back({ {}, stage, module, "main" });
        }

        return stages;
    }

    ShaderLayout* VkShader::Layout(setLocation set) const { return mLayouts.at(set); }

    const std::map<setLocation, ShaderLayout*>& VkShader::Layouts() const { return mLayouts; }

    std::vector<vk::DescriptorSetLayout> VkShader::DescriptorLayouts() const {
        std::vector<vk::DescriptorSetLayout> layouts{};
        for (const auto& [set, layout] : mLayouts) {
            layouts.push_back(reinterpret_cast<VkShaderLayout*>(layout)->Layout());
        }

        return layouts;
    }

    const std::vector<VkShaderPushConstant>& VkShader::PushConstants() const { return mPushConstants; }

    u64 VkShader::SourceId() const { return mSourceId; }
    
    void VkShader::Reload() {
        DestroyShader();
        CreateShader();       
    }

    void VkShader::CreateShader() {
        std::map<setLocation, std::map<u32, ShaderResource*>> reflection{};
        for (const auto& [file, stage] : mSources) {
            vk::ShaderStageFlagBits vkStage{ GetVkShaderStage(stage) };
            
            if (mShaderModules.find(vkStage) != mShaderModules.end()) {
                LOG_ERROR("Compiling shader with duplicate stage");
                continue;
            }

            auto byteCode = CompileShader(file, vkStage);

            vk::ShaderModuleCreateInfo createInfo{};
            createInfo.codeSize = static_cast<u32>(byteCode.size()) * sizeof(u32);
            createInfo.pCode = byteCode.data();
            mShaderModules.insert({ vkStage, VkCore::Instance()->Device().createShaderModule(createInfo) });

            ReflectShader(byteCode, vkStage, reflection);
        }

        for (const auto& [set, resources] : reflection) {
            mLayouts.insert({ set, new VkShaderLayout(resources) });
        }
    }

    void VkShader::DestroyShader() {
        mSourceId = 0;
        
        for (const auto& [set, layout] : mLayouts) {
            delete reinterpret_cast<VkShaderLayout*>(layout);
        }
        mLayouts.clear();

        for (const auto& [stage, module] : mShaderModules) {
            VkCore::Instance()->Device().destroyShaderModule(module);
        }
        mShaderModules.clear();

        mPushConstants.clear();
    }

    std::vector<u32> VkShader::CompileShader(const std::filesystem::path& file, vk::ShaderStageFlagBits stage) {
        static shaderc::Compiler compiler{};
        shaderc::CompileOptions options{};
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);

        auto data{ Utils::ReadFile(file) };
        if (data.empty()) {
            return {};
        }

        std::string code{ reinterpret_cast<char*>(data.data()), data.size() };
        mSourceId += std::hash<std::string>{}(code);
        
        shaderc::SpvCompilationResult result{ compiler.CompileGlslToSpv(code, GetShadercShaderKind(stage), file.filename().string().c_str()) };

        if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
            LOG_ERROR("Failed to compile shader: {}", result.GetErrorMessage());
            LOG_ERROR("Shader file: {}", file.string());
            return {};
        }

        return { result.begin(), result.end() };
    }

    void VkShader::ReflectShader(const std::vector<u32>& shader, vk::ShaderStageFlagBits stage, std::map<setLocation, std::map<u32, ShaderResource*>>& reflection) {
        spirv_cross::Compiler compiler{ shader };
        spirv_cross::ShaderResources resources{ compiler.get_shader_resources() };

        for (const auto& resource : resources.push_constant_buffers) {
            const spirv_cross::SPIRType& type{ compiler.get_type(resource.base_type_id) };
            u32 size{ static_cast<u32>(compiler.get_declared_struct_size(type)) };
            u32 offset{ 0 };

            if (!mPushConstants.empty()) {
                offset = mPushConstants.back().Offset + mPushConstants.back().Size;
            }

            vk::PushConstantRange range{};
            range.stageFlags = stage;
            range.offset = offset;
            range.size = size;

            VkShaderPushConstant& pushConstant{ mPushConstants.emplace_back() };
            pushConstant.Offset = offset;
            pushConstant.Size = size;
            pushConstant.Name = resource.name;
            pushConstant.Range = range;
        }

        for (const auto& resource : resources.uniform_buffers) {
            bindingLocation binding{ compiler.get_decoration(resource.id, spv::DecorationBinding) };
            setLocation set{ compiler.get_decoration(resource.id, spv::DecorationDescriptorSet) };
            const spirv_cross::SPIRType& type{ compiler.get_type(resource.base_type_id) };
            u32 size{ static_cast<u32>(compiler.get_declared_struct_size(type)) };

            vk::DescriptorSetLayoutBinding description{};
            description.binding = binding;
            description.descriptorType = vk::DescriptorType::eUniformBuffer;
            description.descriptorCount = 1;
            description.stageFlags = stage;
            description.pImmutableSamplers = nullptr;

            VkShaderResource* shaderResource{ new VkShaderResource{} };
            shaderResource->Description = description;
            shaderResource->Size = size;
            shaderResource->Type = ShaderResourceType::Uniform;
            shaderResource->Name = resource.name;

            if (reflection.find(set) == reflection.end()) {
                reflection.insert({ set, {} });
            }

            if (reflection.at(set).find(binding) == reflection.at(set).end()) {
                reflection.at(set).insert({ binding, shaderResource });
                continue;
            }

            LOG_WARN("Duplicate resource at set: {}, binding: {} with names: {} and: {}", set, binding, reflection.at(set).at(binding)->Name, shaderResource->Name);
        }

        for (const auto& resource : resources.sampled_images) {
            bindingLocation binding{ compiler.get_decoration(resource.id, spv::DecorationBinding) };
            setLocation set{ compiler.get_decoration(resource.id, spv::DecorationDescriptorSet) };

            vk::DescriptorSetLayoutBinding description{};
            description.binding = binding;
            description.descriptorType = vk::DescriptorType::eCombinedImageSampler;
            description.descriptorCount = 1;
            description.stageFlags = stage;
            description.pImmutableSamplers = nullptr;

            VkShaderResource* shaderResource{ new VkShaderResource{} };
            shaderResource->Description = description;
            shaderResource->Size = 0;
            shaderResource->Type = ShaderResourceType::Image;
            shaderResource->Name = resource.name;
            
            if (reflection.find(set) == reflection.end()) {
                reflection.insert({ set, {} });
            }

            if (reflection.at(set).find(binding) == reflection.at(set).end()) {
                reflection.at(set).insert({ binding, shaderResource });
                continue;
            }

            LOG_WARN("Duplicate resource at set: {}, binding: {} with names: {} and: {}", set, binding, reflection.at(set).at(binding)->Name, shaderResource->Name);
        }
    }
}