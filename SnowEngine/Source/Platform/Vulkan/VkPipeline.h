#pragma once
#include "Graphics/Rhi/Pipeline.h"
#include <vulkan/vulkan.hpp>

namespace Snow {
    class VkPipeline : public Pipeline {
    public:
        VkPipeline(const PipelineCreateInfo& createInfo);
        virtual ~VkPipeline() override;

        virtual void Bind() override;

    private:
        void CreatePipelineLayout(const PipelineCreateInfo& info);
        void CreatePipeline(const PipelineCreateInfo& info);

        vk::PipelineLayout mPipelineLayout;
        vk::Pipeline mPipeline;
    };
}
