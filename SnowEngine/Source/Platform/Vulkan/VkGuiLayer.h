#pragma once
#include <vulkan/vulkan.hpp>
#include <imgui.h>
#include "Graphics/Rhi/GuiLayer.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/Rhi/Surface.h"
#include "VkRenderPass.h"

namespace Snow {
    class VkGuiLayer : public GuiLayer {
    public:
        VkGuiLayer(const RenderTarget& rt);
        virtual ~VkGuiLayer() override;

        virtual void Begin() override;
        virtual void End() override;

    private:
        void CreateResources(Surface* surface);
        void InitImGui(const RenderTarget& rt);

        vk::DescriptorPool mDescriptorPool;
        VkRenderPass* mRenderPass;
        std::vector<ImTextureID> mTextures;
        vk::Sampler mSampler;
    };
}