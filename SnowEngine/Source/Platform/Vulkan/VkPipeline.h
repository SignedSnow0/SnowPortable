#pragma once
#include "Graphics/Rhi/Pipeline.h"
#include "VkShader.h"
#include <vulkan/vulkan.hpp>
#include "Core/ResourcePtr.h"

namespace Snow {
    class VkDescriptorSet;
    
    class VkPipeline : public Pipeline {
    public:
        VkPipeline(const PipelineCreateInfo& createInfo);
        virtual ~VkPipeline() override;

        virtual void Bind() override;
        virtual DescriptorSet* CreateDescriptorSet(u32 setIndex) override;
        virtual void PushConstant(const std::string& name, const void* data, u32 size) override;

    private:
        void CreatePipelineLayout(const PipelineCreateInfo& info);
        void CreatePipeline(const PipelineCreateInfo& info);

        vk::PipelineLayout mPipelineLayout;
        vk::Pipeline mPipeline;
        ResourcePtr<Shader> mShader;
        u64 mShaderId{ 0 };
        PipelineCreateInfo mCreateInfo;
        std::vector<VkDescriptorSet*> mCreatedDescriptorSets;
    };
}
