#include "VkImage.h"
#include "VkCore.h"
#include "Core/Logger.h"
#include <backends/imgui_impl_vulkan.h>

namespace Snow {
    static void ChangeLayout(::vk::Image image, u32 mipLevels, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, u32 layerCount) {
        vk::ImageMemoryBarrier barrier{};
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = layerCount;

        vk::PipelineStageFlags sourceStage;
        vk::PipelineStageFlags destinationStage;
        if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
            barrier.srcAccessMask = {};
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
            barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
        }
        else {
            LOG_ERROR("No image layout transition implemented for this layout combination!");
        }

        VkCore::Instance()->SubmitInstantCommand([&](vk::CommandBuffer cmd) {
            cmd.pipelineBarrier(sourceStage, destinationStage, {}, 0, nullptr, 0, nullptr, 1, &barrier);
        });
    }
    
    VkImage::VkImage(const ImageCreateInfo& createInfo) {
        CreateImage(createInfo);
        CreateViewAndSampler(createInfo);
    }

    VkImage::~VkImage() {
        VkCore::Instance()->Device().destroySampler(mSampler);
        VkCore::Instance()->Device().destroyImageView(mView);

        vmaDestroyImage(VkCore::Instance()->Allocator(), mImage, mAllocation);
    }

    ImTextureID VkImage::GuiId() {
        if (!mGuiId) {
            mGuiId = ImGui_ImplVulkan_AddTexture(mSampler, mView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }

        return mGuiId;
    }

    vk::Image VkImage::Image() const { return mImage; }

    vk::ImageView VkImage::View() const { return mView; }

    vk::Sampler VkImage::Sampler() const { return mSampler; }

    void VkImage::CreateImage(const ImageCreateInfo& info) {
        vk::Extent3D extent{};
        if (info.Usage == ImageUsage::ColorAttachment) {
            extent.width = info.Width;
            extent.height = info.Height;
            extent.depth = 1;
        }

        vk::ImageCreateInfo createInfo{};
        createInfo.imageType = vk::ImageType::e2D;
        createInfo.extent = extent;
        createInfo.mipLevels = 1;
        createInfo.arrayLayers = 1;
        createInfo.format = vk::Format::eR8G8B8A8Srgb;
        createInfo.tiling = vk::ImageTiling::eOptimal;
        createInfo.initialLayout = vk::ImageLayout::eUndefined;
        createInfo.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment;
        createInfo.sharingMode = vk::SharingMode::eExclusive;
        createInfo.samples = vk::SampleCountFlagBits::e1;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        vmaCreateImage(VkCore::Instance()->Allocator(), reinterpret_cast<VkImageCreateInfo*>(&createInfo), &allocInfo, reinterpret_cast<::VkImage*>(&mImage), &mAllocation, nullptr);

        ChangeLayout(mImage, 1, createInfo.format, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal, 1);
    }

    void VkImage::CreateViewAndSampler(const ImageCreateInfo& info) {
        {
            vk::ImageViewCreateInfo createInfo{};
            createInfo.image = mImage;
            createInfo.viewType = vk::ImageViewType::e2D;
         
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (info.Usage == ImageUsage::ColorAttachment) {
                createInfo.format = vk::Format::eR8G8B8A8Srgb;
                createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            }

            mView = VkCore::Instance()->Device().createImageView(createInfo);
        }
        {
            vk::SamplerCreateInfo createInfo{};
            createInfo.magFilter = vk::Filter::eLinear;
            createInfo.minFilter = vk::Filter::eLinear;
            createInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
            createInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
            createInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
            createInfo.mipLodBias = 0.0f;
            createInfo.anisotropyEnable = VK_FALSE;
            createInfo.maxAnisotropy = 1.0f;
            createInfo.compareEnable = VK_FALSE;
            createInfo.compareOp = vk::CompareOp::eAlways;
            createInfo.minLod = 0.0f;
            createInfo.maxLod = 1.0f;
            createInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;

            mSampler = VkCore::Instance()->Device().createSampler(createInfo);
        }
    }
}