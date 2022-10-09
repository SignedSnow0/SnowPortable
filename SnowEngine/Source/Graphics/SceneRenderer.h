#pragma once
#include "Core/Scene.h"
#include "Rhi/Shader.h"
#include "Rhi/RenderPass.h"
#include "Rhi/Pipeline.h"
#include "Rhi/DescriptorSet.h"
#include "RenderTarget.h"
#include "Rhi/Buffers.h"

namespace Snow {
    class SceneRenderer {
    public:
        SceneRenderer(Scene* scene, RenderTarget& target);
        ~SceneRenderer();

        RenderPass* OutputRenderPass();
        
        void Render();

    private:
        Scene* mScene{ nullptr };

        Shader* mShader{ nullptr };
        RenderPass* mRenderPass{ nullptr };
        Pipeline* mPipeline{ nullptr };
        DescriptorSet* mDescriptorSet{ nullptr };
        VertexBuffer* mVertexBuffer{ nullptr };
        IndexBuffer* mIndexBuffer{ nullptr };
    };
}