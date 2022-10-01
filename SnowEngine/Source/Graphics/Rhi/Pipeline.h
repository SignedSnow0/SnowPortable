#pragma once
#include "Shader.h"
#include "RenderPass.h"
#include "DescriptorSet.h"

namespace Snow {
    struct PipelineCreateInfo {
        Shader* ShaderProgram;
        RenderPass* TargetPass;
    };

    class Pipeline {
    public:
        static Pipeline* Create(const PipelineCreateInfo& createInfo);
        virtual ~Pipeline() = default;

        virtual void Bind() = 0;
        virtual DescriptorSet* CreateDescriptorSet(u32 setIndex) = 0;

    protected:
        Pipeline() = default;
    };
}