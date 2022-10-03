#pragma once
#include <vulkan/vulkan.hpp>
#include "Graphics/Rhi/RenderPass.h"
#include "VkSurface.h"

namespace Snow {
    class VkRenderPass : public RenderPass {
    public:
        VkRenderPass(const RenderPassCreateInfo& createInfo);
        virtual ~VkRenderPass() override;

        vk::RenderPass RenderPass() const;
            
        virtual u32 Width() const override;
        virtual u32 Height() const override;
        virtual const std::vector<Image*>& Images() const override;

        virtual void Begin() override;
        virtual void End() override;
        void Resize(u32 width, u32 height);

    private:
        void CreateRenderPass(const RenderPassCreateInfo& info);
        void CreateFramebuffers(u32 frame);

        vk::RenderPass mRenderPass;
        std::vector<vk::Framebuffer> mFramebuffers;
        std::vector<Image*> mImages;
        u32 mWidth;
        u32 mHeight;
        VkSurface* mSurface{ nullptr };
    };
}