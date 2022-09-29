#include "VkRenderPass.h"
#include "VkCore.h"

namespace Snow {
    VkRenderPass::VkRenderPass(const RenderPassCreateInfo& createInfo)
        : mWidth{ createInfo.InitialWidth }, mHeight{ createInfo.InitialHeight }, mSurface{ reinterpret_cast<VkSurface*>(createInfo.SurfaceOutput) } {
        CreateRenderPass(createInfo);
        CreateFramebuffers();
    }

    VkRenderPass::~VkRenderPass() {
        DestroyResizableObjects();
        
        VkCore::Instance()->Device().destroyRenderPass(mRenderPass);
    }

    vk::RenderPass VkRenderPass::RenderPass() const { return mRenderPass; }
    
    u32 VkRenderPass::Width() const { return mWidth; }

    u32 VkRenderPass::Height() const { return mHeight; }

    void VkRenderPass::Begin() {
        if (mSurface->Width() != mWidth || mSurface->Height() != mHeight) {
            Resize();
        }
        
        vk::ClearValue clearColor = vk::ClearColorValue(std::array<f32, 4>{ 0.0f, 0.0f, 0.0f, 1.0f });
        
        vk::RenderPassBeginInfo renderPassInfo;
        renderPassInfo.renderPass = mRenderPass;
        renderPassInfo.framebuffer = mFramebuffers[VkSurface::BoundSurface()->CurrentFrame()];
        renderPassInfo.renderArea.offset = vk::Offset2D(0, 0);
        renderPassInfo.renderArea.extent = vk::Extent2D(mWidth, mHeight);
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        VkSurface::BoundSurface()->CommandBuffer().beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

        vk::Viewport viewport;
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<f32>(mWidth);
        viewport.height = static_cast<f32>(mHeight);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkSurface::BoundSurface()->CommandBuffer().setViewport(0, 1, &viewport);

        vk::Rect2D scissor;
        scissor.offset = vk::Offset2D(0, 0);
        scissor.extent = vk::Extent2D(mWidth, mHeight);

        VkSurface::BoundSurface()->CommandBuffer().setScissor(0, 1, &scissor);
    }

    void VkRenderPass::End() {
        VkSurface::BoundSurface()->CommandBuffer().endRenderPass();
    }

    void VkRenderPass::CreateRenderPass(const RenderPassCreateInfo& info) {
        vk::AttachmentDescription colorAttachment;
        colorAttachment.format = vk::Format::eB8G8R8A8Unorm;
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

        vk::AttachmentReference colorAttachmentRef;
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::SubpassDescription subpass;
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        vk::SubpassDependency dependency;
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dependency.srcAccessMask = vk::AccessFlags();
        dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
        
        vk::RenderPassCreateInfo createInfo;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = &colorAttachment;
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &subpass;
        createInfo.dependencyCount = 1;
        createInfo.pDependencies = &dependency;

        mRenderPass = VkCore::Instance()->Device().createRenderPass(createInfo);
    }

    void VkRenderPass::CreateFramebuffers() {
        std::vector<vk::ImageView> views{ mSurface->Images() };
        mFramebuffers.resize(views.size());

        u32 i{ 0 };
        for (const auto view : views) {
            vk::FramebufferCreateInfo createInfo;
            createInfo.renderPass = mRenderPass;
            createInfo.attachmentCount = 1;
            createInfo.width = mSurface->Width();
            createInfo.height = mSurface->Height();
            createInfo.layers = 1;
            createInfo.pAttachments = &view;

            mFramebuffers[i] = VkCore::Instance()->Device().createFramebuffer(createInfo);
            i++;
        }
    }

    void VkRenderPass::DestroyResizableObjects() {
        for (auto fb : mFramebuffers) {
            VkCore::Instance()->Device().destroyFramebuffer(fb);
        }
    }

    void VkRenderPass::Resize() {
        DestroyResizableObjects();

        mWidth = mSurface->Width();
        mHeight = mSurface->Height();
        CreateFramebuffers();
    }
}