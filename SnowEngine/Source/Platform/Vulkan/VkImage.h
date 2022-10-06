#pragma once
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>
#include "Graphics/Rhi/Image.h"

namespace Snow {
    class VkImage : public Image {
    public:
        VkImage(const ImageCreateInfo& createInfo);
        virtual ~VkImage() override;

        virtual ImTextureID GuiId() override;
        
        vk::Image Image() const;
        vk::ImageView View() const;
        vk::Sampler Sampler() const;

    private:
        void CreateImage(const ImageCreateInfo& info);
        void CreateViewAndSampler(const ImageCreateInfo& info);

        vk::Image mImage;
        vk::ImageView mView;
        vk::Sampler mSampler;
        VmaAllocation mAllocation;
        ImTextureID mGuiId{ nullptr };
        ImageUsage mUsage;
    };
}