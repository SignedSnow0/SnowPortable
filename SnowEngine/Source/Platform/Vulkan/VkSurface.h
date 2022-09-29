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
        std::vector<vk::ImageView>& Images();
        u32 Width() const;
        u32 Height() const;

        virtual void Begin() override;
        virtual void End() override;
        
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
         
        static VkSurface* sBoundSurface;
    };
}