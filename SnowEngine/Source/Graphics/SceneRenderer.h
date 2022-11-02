#pragma once
#include "Core/Scene.h"
#include "Rhi/Shader.h"
#include "Rhi/RenderPass.h"
#include "Rhi/Pipeline.h"
#include "Rhi/DescriptorSet.h"
#include "RenderTarget.h"
#include "Rhi/Buffers.h"
#include "Camera.h"
#include "Mesh.h"

namespace Snow {
    class SceneRenderer {
    public:
        SceneRenderer(RenderTarget& target);
        ~SceneRenderer();

        RenderPass* OutputRenderPass();
        void SetScene(Scene* scene);
        
        void Update();
        void Render();

        static Pipeline* DefaultPipeline();

    private:
        Scene* mScene{ nullptr };

        ResourcePtr<Shader> mShader{ nullptr };
        RenderPass* mRenderPass{ nullptr };
        Pipeline* mPipeline{ nullptr };
        ResourcePtr<DescriptorSet> mDescriptorSet{ nullptr };
		
        Camera mCamera;

        static SceneRenderer* sInstance;
    };
}