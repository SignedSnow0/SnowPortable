#include "VkSurface.h"
#include <GLFW/glfw3.h>
#include "VkCore.h"
#include "Core/Logger.h"

namespace Snow {
    VkSurface* VkSurface::sBoundSurface{ nullptr };
    
    VkSurface::VkSurface(const Window& window) {
        CreateSurface(window);
        ChooseSurfaceSettings();
        CreateSwapChain();
        CreateCommandPool();
        CreateSyncObjects();
    }

    VkSurface::~VkSurface() {
        VkCore::Instance()->Device().waitIdle();
        
        for (u32 i{ 0 }; i < mBackBufferFrames; i++) {
            VkCore::Instance()->Device().destroySemaphore(mBackBuffers[i].ImageAvailable);
            VkCore::Instance()->Device().destroySemaphore(mBackBuffers[i].RenderFinished);
            VkCore::Instance()->Device().destroyFence(mBackBuffers[i].InFlight);
        }

        VkCore::Instance()->Device().destroyCommandPool(mCommandPool);
        
        DestroyResizableObjects();
        
        VkCore::Instance()->VkInstance().destroySurfaceKHR(mSurface);
    }

    vk::CommandBuffer VkSurface::CommandBuffer() const { return mBackBuffers[mRenderingFrame].CmdBuffer; }

    u32 VkSurface::CurrentFrame() const { return mCurrentFrame; }

    std::vector<vk::ImageView>& VkSurface::Images() { return mSwapChainImageViews; }

    u32 VkSurface::Width() const { return mExtent.width; }

    u32 VkSurface::Height() const { return mExtent.height; }

    void VkSurface::Begin() {
        VkCore::Instance()->Device().waitForFences(mBackBuffers[mRenderingFrame].InFlight, VK_TRUE, UINT64_MAX);

        try {
            VkCore::Instance()->Device().acquireNextImageKHR(mSwapChain, UINT64_MAX, mBackBuffers[mRenderingFrame].ImageAvailable, nullptr, &mCurrentFrame);
        }
        catch (vk::OutOfDateKHRError) {
            Resize();
            return;
        }

        VkCore::Instance()->Device().resetFences(mBackBuffers[mRenderingFrame].InFlight);

        static vk::CommandBufferBeginInfo beginInfo;

        mBackBuffers[mRenderingFrame].CmdBuffer.begin(beginInfo);

        sBoundSurface = this;
    }

    void VkSurface::End() {
        mBackBuffers[mRenderingFrame].CmdBuffer.end();

        sBoundSurface = nullptr;

        vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

        vk::SubmitInfo submitInfo;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &mBackBuffers[mRenderingFrame].ImageAvailable;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &mBackBuffers[mRenderingFrame].CmdBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &mBackBuffers[mRenderingFrame].RenderFinished;

        VkCore::Instance()->QueueFamilyIndices().Graphics.second.submit(submitInfo, mBackBuffers[mRenderingFrame].InFlight);

        vk::PresentInfoKHR presentInfo;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &mBackBuffers[mRenderingFrame].RenderFinished;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &mSwapChain;
        presentInfo.pImageIndices = &mCurrentFrame;
        presentInfo.pResults = nullptr;

        try {
            VkCore::Instance()->QueueFamilyIndices().Present.second.presentKHR(presentInfo);
        }
        catch (vk::OutOfDateKHRError) {
            Resize();
        }
        
        mRenderingFrame = (mRenderingFrame + 1) % mBackBufferFrames;
    }

    VkSurface* VkSurface::BoundSurface() { return sBoundSurface; }
    
    void VkSurface::CreateSurface(const Window& window) {
        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface(VkCore::Instance()->VkInstance(), reinterpret_cast<GLFWwindow*>(window.Native()), nullptr, &surface) != VK_SUCCESS) {
            assert(false && "Failed to create window surface");
        }

        mSurface = surface;
    }

    void VkSurface::ChooseSurfaceSettings() {
        vk::SurfaceCapabilitiesKHR capabilities = VkCore::Instance()->PhysicalDevice().getSurfaceCapabilitiesKHR(mSurface);
        std::vector<vk::SurfaceFormatKHR> formats = VkCore::Instance()->PhysicalDevice().getSurfaceFormatsKHR(mSurface);
        std::vector<vk::PresentModeKHR> presentModes = VkCore::Instance()->PhysicalDevice().getSurfacePresentModesKHR(mSurface);

        mSurfaceFormat = formats[0];
        for (const auto& [format, colorSpace] : formats) {
            if (format == vk::Format::eB8G8R8A8Unorm && colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                mSurfaceFormat = format;
                break;
            }
        }

        mPresentMode = vk::PresentModeKHR::eFifo;
        for (const auto& presentMode : presentModes) {
            if (presentMode == vk::PresentModeKHR::eMailbox) {
                mPresentMode = presentMode;
                break;
            }
        }

        mImageCount = capabilities.minImageCount;
        if (mImageCount == 1 && capabilities.maxImageCount >= 2)
            mImageCount = 2;
        mBackBufferFrames = mImageCount - 1;
        mBackBuffers.resize(mBackBufferFrames);
    }

    void VkSurface::QuerySurfaceExtent() {
        vk::SurfaceCapabilitiesKHR capabilities = VkCore::Instance()->PhysicalDevice().getSurfaceCapabilitiesKHR(mSurface);
        if (capabilities.currentExtent.width != UINT32_MAX) {
            mExtent = capabilities.currentExtent;
        }
        else {
            mExtent = vk::Extent2D{ 800, 600 };
            //TODO: get size from host window
        }
    }

    void VkSurface::CreateSwapChain() {
        QuerySurfaceExtent();

        vk::SurfaceCapabilitiesKHR capabilities = VkCore::Instance()->PhysicalDevice().getSurfaceCapabilitiesKHR(mSurface);
        const QueueFamilies& families = VkCore::Instance()->QueueFamilyIndices();

        b8 shared{ families.Present.first == families.Graphics.first };
        u32 queueFamilyIndices[] = { families.Graphics.first, families.Present.first };

        vk::SwapchainCreateInfoKHR createInfo{};
        createInfo.surface = mSurface;
        createInfo.minImageCount = mImageCount;
        createInfo.imageFormat = mSurfaceFormat.format;
        createInfo.imageColorSpace = mSurfaceFormat.colorSpace;
        createInfo.imageExtent = mExtent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
        createInfo.preTransform = capabilities.currentTransform;
        createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        createInfo.presentMode = mPresentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = nullptr;
        createInfo.imageSharingMode = shared ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = shared ? 0 : 2;
        createInfo.pQueueFamilyIndices = shared ? nullptr : queueFamilyIndices;

        mSwapChain = VkCore::Instance()->Device().createSwapchainKHR(createInfo);

        mSwapChainImages = VkCore::Instance()->Device().getSwapchainImagesKHR(mSwapChain);

        CreateImageViews();
    }

    void VkSurface::CreateImageViews() {
        mSwapChainImageViews.resize(mSwapChainImages.size());

        for (u32 i{ 0 }; i < mSwapChainImages.size(); i++) {
            vk::ImageViewCreateInfo createInfo{};
            createInfo.image = mSwapChainImages[i];
            createInfo.viewType = vk::ImageViewType::e2D;
            createInfo.format = mSurfaceFormat.format;
            createInfo.components.r = vk::ComponentSwizzle::eIdentity;
            createInfo.components.g = vk::ComponentSwizzle::eIdentity;
            createInfo.components.b = vk::ComponentSwizzle::eIdentity;
            createInfo.components.a = vk::ComponentSwizzle::eIdentity;
            createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            mSwapChainImageViews[i] = VkCore::Instance()->Device().createImageView(createInfo);
        }
    }

    void VkSurface::CreateCommandPool() {
        vk::CommandPoolCreateInfo poolInfo{};
        poolInfo.queueFamilyIndex = VkCore::Instance()->QueueFamilyIndices().Graphics.first;
        poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

        mCommandPool = VkCore::Instance()->Device().createCommandPool(poolInfo);

        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.commandPool = mCommandPool;
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount = mBackBufferFrames;

        auto cmds{ VkCore::Instance()->Device().allocateCommandBuffers(allocInfo) };
        for (u32 i{ 0 }; i < mBackBuffers.size(); i++) {
            mBackBuffers[i].CmdBuffer = cmds[i];
        }
    }

    void VkSurface::CreateSyncObjects() {
        vk::SemaphoreCreateInfo semaphoreInfo{};
        vk::FenceCreateInfo fenceInfo{};
        fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

        for (u32 i{ 0 }; i < mBackBufferFrames; i++) {
            mBackBuffers[i].ImageAvailable = VkCore::Instance()->Device().createSemaphore(semaphoreInfo);
            mBackBuffers[i].RenderFinished = VkCore::Instance()->Device().createSemaphore(semaphoreInfo);
            mBackBuffers[i].InFlight = VkCore::Instance()->Device().createFence(fenceInfo);
        }
    }

    void VkSurface::DestroyResizableObjects() {
        for (auto& view : mSwapChainImageViews) {
            VkCore::Instance()->Device().destroyImageView(view);
        }

        VkCore::Instance()->Device().destroySwapchainKHR(mSwapChain);
    }

    void VkSurface::Resize() {
        VkCore::Instance()->Device().waitIdle();

        DestroyResizableObjects();

        CreateSwapChain();
    }
}