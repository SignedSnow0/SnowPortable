#pragma once
#include "Graphics/Rhi/RenderPass.h"
#include <vulkan/vulkan.hpp>
#include "VkSurface.h"

namespace Snow {
    class VkRenderPass : public RenderPass {
    public:
        VkRenderPass(const RenderPassCreateInfo& createInfo);
        virtual ~VkRenderPass() override;

        vk::RenderPass RenderPass() const;
            
        virtual u32 Width() const override;
        virtual u32 Height() const override;

        virtual void Begin() override;
        virtual void End() override;

    private:
        void CreateRenderPass(const RenderPassCreateInfo& info);
        void CreateFramebuffers();
        void DestroyResizableObjects();
        void Resize();

        vk::RenderPass mRenderPass;
        std::vector<vk::Framebuffer> mFramebuffers;
        u32 mWidth;
        u32 mHeight;
        VkSurface* mSurface{ nullptr };
    };
}