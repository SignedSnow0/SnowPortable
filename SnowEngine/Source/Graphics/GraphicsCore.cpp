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

    void Graphics::CreateDefaultResources(Surface* surface) {
        sInstance->mDefaultResources.ShaderProgram = Shader::Create({
            { Utils::GetShadersPath() / "default.vert", ShaderStage::Vertex },
            { Utils::GetShadersPath() / "default.frag", ShaderStage::Fragment }
        });

        sInstance->mDefaultResources.TargetPass = RenderPass::Create({ 1280, 720, 2, surface });

        sInstance->mDefaultResources.GraphicsPipeline = Pipeline::Create({ sInstance->mDefaultResources.ShaderProgram, sInstance->mDefaultResources.TargetPass });

        sInstance->mDefaultResources.VertexBuffer = VertexBuffer::Create(vertices.data(), vertices.size());
        sInstance->mDefaultResources.IndexBuffer = IndexBuffer::Create(indices.data(), indices.size());
    }

    void Graphics::DebugDraw() {
        //TODO: remove this
        sInstance->mDefaultResources.TargetPass->Begin();
        sInstance->mDefaultResources.GraphicsPipeline->Bind();

        sInstance->mDefaultResources.VertexBuffer->Bind();
        sInstance->mDefaultResources.IndexBuffer->Bind();
        sInstance->mDefaultResources.IndexBuffer->Draw();

        sInstance->mDefaultResources.TargetPass->End();
    }
}