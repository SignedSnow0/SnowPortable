#pragma once
#include "Graphics/Rhi/Pipeline.h"
#include "VkShader.h"
#include <vulkan/vulkan.hpp>

namespace Snow {
    class VkPipeline : public Pipeline {
    public:
        VkPipeline(const PipelineCreateInfo& createInfo);
        virtual ~VkPipeline() override;

        virtual void Bind() override;
        virtual DescriptorSet* CreateDescriptorSet(u32 setIndex) override;
        
    private:
        void CreatePipelineLayout(const PipelineCreateInfo& info);
        void CreatePipeline(const PipelineCreateInfo& info);

        vk::PipelineLayout mPipelineLayout;
        vk::Pipeline mPipeline;
        VkShader* mShader;
    };
}
