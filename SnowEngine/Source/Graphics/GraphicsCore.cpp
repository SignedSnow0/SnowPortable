#include "GraphicsCore.h"
#include "Core/Logger.h"
#include "Core/Utils.h"
#include "Graphics/Rhi/Shader.h"

#include "Platform/Vulkan/VkCore.h"

namespace Snow {
    Graphics* Graphics::sInstance{ nullptr };

    static const std::vector<Vertex> vertices = {
        { { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
        { {  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
        { {  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f } },
        { { -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } }
    };

    static const std::vector<u32> indices = {
        0, 1, 2, 2, 3, 0
    };

    b8 Graphics::Initialize(const GraphicsAPI& api) {
        if (sInstance) {
            LOG_WARN("Graphics system already initialized");
            return false;
        }

        switch (api) {
            case GraphicsAPI::Vulkan: {
                sInstance = new VkCore();
                LOG_INFO("Graphics system initialized using Vulkan");
                
                break;
            }
            default: {
                LOG_ERROR("Invalid graphics API");
                return false;
            }
        }
        sInstance->mApi = api;

        return true;
    }

    void Graphics::Shutdown() {
        if (!sInstance) {
            return;
        }

        delete sInstance->mDefaultResources.DescriptorSet;
        
        delete sInstance->mDefaultResources.VertexBuffer;
        delete sInstance->mDefaultResources.IndexBuffer;

        delete sInstance->mDefaultResources.GraphicsPipeline;
        delete sInstance->mDefaultResources.TargetPass;
        delete sInstance->mDefaultResources.ShaderProgram;

        delete sInstance;
        sInstance = nullptr;
        LOG_INFO("Graphics system shutdown");
    }

    GraphicsAPI Graphics::Api() { return sInstance->mApi; }

    RenderPass* Graphics::CreateDefaultResources(Surface* surface) {
        sInstance->mDefaultResources.ShaderProgram = Shader::Create({
            { Utils::GetShadersPath() / "default.vert", ShaderStage::Vertex },
            { Utils::GetShadersPath() / "default.frag", ShaderStage::Fragment }
        });

        sInstance->mDefaultResources.TargetPass = RenderPass::Create({ RenderPassUsage::Image, surface->Width(), surface->Height(), 2, nullptr });

        sInstance->mDefaultResources.GraphicsPipeline = Pipeline::Create({ sInstance->mDefaultResources.ShaderProgram, sInstance->mDefaultResources.TargetPass });

        sInstance->mDefaultResources.VertexBuffer = VertexBuffer::Create(vertices.data(), vertices.size());
        sInstance->mDefaultResources.IndexBuffer = IndexBuffer::Create(indices.data(), indices.size());

        sInstance->mDefaultResources.DescriptorSet = sInstance->mDefaultResources.GraphicsPipeline->CreateDescriptorSet(0);

        return sInstance->mDefaultResources.TargetPass;
    }

    void Graphics::DebugDraw() {
        //TODO: remove this
        sInstance->mDefaultResources.TargetPass->Begin();
        sInstance->mDefaultResources.GraphicsPipeline->Bind();

        sInstance->mDefaultResources.DescriptorSet->Bind();

        static struct Camera{
            mat4f View;
            mat4f Projection;
        } camera;

        camera.View = glm::lookAt(vec3f{ 2.0f, 2.0f, 2.0f }, vec3f{ 0.0f, 0.0f, 0.0f }, vec3f{ 0.0f, 0.0f, 1.0f });
        camera.Projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 10.0f);

        sInstance->mDefaultResources.DescriptorSet->SetUniform("Camera", &camera);

        sInstance->mDefaultResources.VertexBuffer->Bind();
        sInstance->mDefaultResources.IndexBuffer->Bind();
        sInstance->mDefaultResources.IndexBuffer->Draw();

        sInstance->mDefaultResources.TargetPass->End();
    }
}