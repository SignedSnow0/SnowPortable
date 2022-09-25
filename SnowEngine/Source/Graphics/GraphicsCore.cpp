#include "GraphicsCore.h"
#include "Core/Logger.h"
#include "Core/Utils.h"
#include "Graphics/Rhi/Shader.h"

#include "Platform/Vulkan/VkCore.h"

namespace Snow {
    Graphics* Graphics::sInstance{ nullptr };

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
    }

    void Graphics::DebugDraw() {
        //TODO: remove this
        sInstance->mDefaultResources.TargetPass->Begin();
        sInstance->mDefaultResources.GraphicsPipeline->Bind();


        
        sInstance->mDefaultResources.TargetPass->End();
    }
}