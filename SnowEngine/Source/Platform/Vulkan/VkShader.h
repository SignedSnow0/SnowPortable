#pragma once
#include <vulkan/vulkan.hpp>
#include "Graphics/Rhi/Shader.h"

namespace Snow {
    //TODO: See if vk specific members can be created on request in order to remove inheritance from shader resources
    struct VkShaderResource : public ShaderResource {
        vk::DescriptorSetLayoutBinding Description;
    };

    struct VkShaderPushConstant : public ShaderPushConstant {
        vk::PushConstantRange Range;
    };

    struct VkShaderLayout : public ShaderLayout {
        VkShaderLayout(const std::map<u32, ShaderResource*>& resources);
        ~VkShaderLayout();
        
        virtual const std::map<u32, ShaderResource*>& VkShaderLayout::Resources() const override;
        vk::DescriptorSetLayout Layout() const;

    private:
        vk::DescriptorSetLayout mLayout;
        std::map<u32, ShaderResource*> mResources;
    };

    class VkShader : public Shader {
    public:
        VkShader(const std::vector<std::pair<std::filesystem::path, ShaderStage>>& shaders);
        virtual ~VkShader() override;

        std::vector<vk::PipelineShaderStageCreateInfo> ShaderStages() const;
        ShaderLayout* Layout(setLocation set) const;
        virtual const std::map<setLocation, ShaderLayout*>& Layouts() const override;
        std::vector<vk::DescriptorSetLayout> DescriptorLayouts() const;
        const std::vector<VkShaderPushConstant>& PushConstants() const;
        u64 SourceId() const;
        
        virtual void Reload() override;

    private:
        void CreateShader();
        void DestroyShader();
        
        std::vector<u32> CompileShader(const std::filesystem::path& file, vk::ShaderStageFlagBits stage);
        void ReflectShader(const std::vector<u32>& shader, vk::ShaderStageFlagBits stage, std::map<setLocation, std::map<u32, ShaderResource*>>& reflection);
        
        std::map<vk::ShaderStageFlagBits, vk::ShaderModule> mShaderModules;
        std::map<setLocation, ShaderLayout*> mLayouts;
        std::vector<VkShaderPushConstant> mPushConstants;
        std::vector<std::pair<std::filesystem::path, ShaderStage>> mSources;
        u64 mSourceId{ 0 };
    };
}