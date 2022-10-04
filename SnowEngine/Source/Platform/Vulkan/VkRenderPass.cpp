#include "VkRenderPass.h"
#include "VkCore.h"
#include "VkImage.h"
#include "Core/Logger.h"

namespace Snow {
    VkRenderPass::VkRenderPass(const RenderPassCreateInfo& createInfo)
        : mWidth{ createInfo.InitialWidth }, mHeight{ createInfo.InitialHeight }, mSurface{ createInfo.SurfaceOutput ? reinterpret_cast<VkSurface*>(createInfo.SurfaceOutput) : nullptr } {
        CreateRenderPass(createInfo);
        if (createInfo.Usage == RenderPassUsage::Image) {
            mImages.resize(createInfo.ImageCount);
            for (u32 i{ 0 }; i < createInfo.ImageCount; i++) {
                mImages[i] = new VkImage({ ImageUsage::ColorAttachment, "", mWidth, mHeight });
            }
        }

        mFramebuffers.resize(createInfo.ImageCount);
        for (u32 i{ 0 }; i < createInfo.ImageCount; i++) {
            CreateFramebuffers(i);
        }
    }

    VkRenderPass::~VkRenderPass() {
        for (auto fb : mFramebuffers) {
            VkCore::Instance()->Device().destroyFramebuffer(fb);
        }

        for (auto* image : mImages) {
            delete image;
        }
        
        VkCore::Instance()->Device().destroyRenderPass(mRenderPass);
    }

    vk::RenderPass VkRenderPass::RenderPass() const { return mRenderPass; }
    
    u32 VkRenderPass::Width() const { return mWidth; }

    u32 VkRenderPass::Height() const { return mHeight; }

    const std::vector<Image*>& VkRenderPass::Images() const { return mImages; }

    void VkRenderPass::Begin() {
        if (mSurface && (mSurface->Width() != mWidth || mSurface->Height() != mHeight)) {
            ResizeSurface();
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

    void VkRenderPass::Resize(u32 width, u32 height) {
        mWidth = width;
        mHeight = height;

        VkSurface::BoundSurface()->AddToDeletionQueue([this](u32 frameIndex) {
            VkCore::Instance()->Device().destroyFramebuffer(mFramebuffers[frameIndex]);

            delete mImages[frameIndex];

            mImages[frameIndex] = new VkImage({ ImageUsage::ColorAttachment, "", mWidth, mHeight });
            
            CreateFramebuffers(frameIndex);
        });
    }

    void VkRenderPass::CreateRenderPass(const RenderPassCreateInfo& info) {
        vk::AttachmentDescription colorAttachment;
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;

        if (info.Usage == RenderPassUsage::Presentation) {
            colorAttachment.format = vk::Format::eB8G8R8A8Unorm;
            colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;
        }
        else if (info.Usage == RenderPassUsage::Image) {
            colorAttachment.format = vk::Format::eR8G8B8A8Srgb;
            colorAttachment.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        }
        
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
        dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

        if (info.Usage == RenderPassUsage::Presentation) {
            dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            dependency.srcAccessMask = vk::AccessFlags();
        }
        else if (info.Usage == RenderPassUsage::Image) {
            dependency.srcStageMask = vk::PipelineStageFlagBits::eFragmentShader;
            dependency.srcAccessMask = vk::AccessFlagBits::eShaderRead;
        }
        
        vk::RenderPassCreateInfo createInfo;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = &colorAttachment;
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &subpass;
        createInfo.dependencyCount = 1;
        createInfo.pDependencies = &dependency;

        mRenderPass = VkCore::Instance()->Device().createRenderPass(createInfo);
    }

    void VkRenderPass::CreateFramebuffers(u32 frame) {
        vk::ImageView view{ mImages.empty() ? mSurface->Images()[frame] : reinterpret_cast<VkImage*>(mImages[frame])->View()};

        vk::FramebufferCreateInfo createInfo;
        createInfo.renderPass = mRenderPass;
        createInfo.attachmentCount = 1;
        createInfo.width = mWidth;
        createInfo.height = mHeight;
        createInfo.layers = 1;
        createInfo.pAttachments = &view;

        mFramebuffers[frame] = VkCore::Instance()->Device().createFramebuffer(createInfo);
    }

    void VkRenderPass::ResizeSurface() {
        mWidth = mSurface->Width();
        mHeight = mSurface->Height();

        for (u32 i{ 0 }; i < mSurface->ImageCount();i++) {
            VkCore::Instance()->Device().destroyFramebuffer(mFramebuffers[i]);
            
            CreateFramebuffers(i);
        }
    }
}