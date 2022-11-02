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
        virtual std::filesystem::path Path() const override;
        
        vk::Image Image() const;
        vk::ImageView View() const;
        vk::Sampler Sampler() const;
        vk::ImageLayout Layout() const;

        void ChangeLayout(vk::ImageLayout newLayout);
        
    private:
        void CreateImage(const ImageCreateInfo& info);
        void CreateViewAndSampler(const ImageCreateInfo& info);

        vk::Image mImage;
        vk::ImageView mView;
        vk::Sampler mSampler;
        vk::Format mFormat;
        vk::ImageLayout mLayout;
        VmaAllocation mAllocation;
        ImTextureID mGuiId{ nullptr };
        ImageUsage mUsage;
        std::filesystem::path mPath{};
    };
}