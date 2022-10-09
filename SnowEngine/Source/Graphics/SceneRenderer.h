#pragma once
#include "Core/Scene.h"
#include "Rhi/Shader.h"
#include "Rhi/RenderPass.h"
#include "Rhi/Pipeline.h"
#include "Rhi/DescriptorSet.h"
#include "RenderTarget.h"
#include "Rhi/Buffers.h"
#include "Camera.h"

namespace Snow {
    class SceneRenderer {
    public:
        SceneRenderer(Scene* scene, RenderTarget& target);
        ~SceneRenderer();

        RenderPass* OutputRenderPass();

        void Update();
        void Render();

    private:
        Scene* mScene{ nullptr };

        Shader* mShader{ nullptr };
        RenderPass* mRenderPass{ nullptr };
        Pipeline* mPipeline{ nullptr };
        DescriptorSet* mDescriptorSet{ nullptr };
        VertexBuffer* mVertexBuffer{ nullptr };
        IndexBuffer* mIndexBuffer{ nullptr };

        Camera mCamera;
    };
}