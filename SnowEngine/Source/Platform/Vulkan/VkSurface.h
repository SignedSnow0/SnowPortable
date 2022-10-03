#pragma once
#include <vulkan/vulkan.hpp>
#include "Graphics/Rhi/Surface.h"

namespace Snow {
    class VkSurface : public Surface {
    public:
        VkSurface(const Window& window);
        virtual ~VkSurface() override;
        
        vk::CommandBuffer CommandBuffer() const;
        u32 CurrentFrame() const;
        u32 ImageCount() const;
        std::vector<vk::ImageView>& Images();
        virtual u32 Width() const override;
        virtual u32 Height() const override;

        virtual void Begin() override;
        virtual void End() override;
        void AddToDeletionQueue(std::function<void(u32 frameIndex)> fn);
        
        static VkSurface* BoundSurface();
        
    private:
        void CreateSurface(const Window& window);
        void ChooseSurfaceSettings();
        void QuerySurfaceExtent();
        void CreateSwapChain();
        void CreateImageViews();
        void CreateCommandPool();
        void CreateSyncObjects();
        void DestroyResizableObjects();
        void Resize();

        vk::SurfaceKHR mSurface;
        vk::SurfaceFormatKHR mSurfaceFormat; //format.format is same as image format
        vk::PresentModeKHR mPresentMode;
        vk::Extent2D mExtent;
        //Number of frames in the swapchain (rendering + present)
        u32 mImageCount;
        vk::SwapchainKHR mSwapChain;
        std::vector<vk::Image> mSwapChainImages;
        std::vector<vk::ImageView> mSwapChainImageViews;
        //Current swapchain image
        u32 mCurrentFrame{ 0 };
        vk::CommandPool mCommandPool;

        struct BackBuffer {
            vk::CommandBuffer CmdBuffer;
            vk::Semaphore ImageAvailable;
            vk::Semaphore RenderFinished;
            vk::Fence InFlight;
        };
        std::vector<BackBuffer> mBackBuffers;
        //Current frame in rendering ops
        u32 mRenderingFrame{ 0 };
        //Number of frames in bakc buffer (swapchain - 1 for presentation)
        u32 mBackBufferFrames;
        std::vector<std::pair<u32, std::function<void(u32 frameIndex)>>> mDeletionQueue;

        static VkSurface* sBoundSurface;
    };
}