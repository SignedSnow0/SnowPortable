#include "VkImage.h"
#include <backends/imgui_impl_vulkan.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "VkCore.h"
#include "Core/Logger.h"
#include "VkBuffers.h"
#include "VkSurface.h"

namespace Snow {
    static void ChangeImageLayout(::vk::Image image, u32 mipLevels, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, u32 layerCount) {
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

        vk::PipelineStageFlags sourceStage{};
        vk::PipelineStageFlags destinationStage{};

        //Source
        if (oldLayout == vk::ImageLayout::eUndefined) {
            barrier.srcAccessMask = {};
            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        }
        else if (oldLayout == vk::ImageLayout::eTransferDstOptimal) {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            sourceStage = vk::PipelineStageFlagBits::eTransfer;
        }

        //Destination
        if (newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;

            barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        }
        else if (newLayout == vk::ImageLayout::eTransferDstOptimal) {
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;

            barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        }
        else if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
            barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
            destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;

            barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
        }

        VkCore::Instance()->SubmitInstantCommand([&](vk::CommandBuffer cmd) {
            cmd.pipelineBarrier(sourceStage, destinationStage, {}, 0, nullptr, 0, nullptr, 1, &barrier);
        });
    }
    
    VkImage::VkImage(const ImageCreateInfo& createInfo)
        : mUsage{ createInfo.Usage } {
        CreateImage(createInfo);
        CreateViewAndSampler(createInfo);
    }

    VkImage::~VkImage() {
        if (mUsage == ImageUsage::Image) {
            VkCore::Instance()->Device().waitIdle();
        }

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

    vk::ImageLayout VkImage::Layout() const { return mLayout; }
    
    void VkImage::ChangeLayout(vk::ImageLayout newLayout) {
        ChangeImageLayout(mImage, 1, mFormat, mLayout, newLayout, 1);
        mLayout = newLayout;
    }

    void VkImage::CreateImage(const ImageCreateInfo& info) {
        i32 width = info.Width;
        i32 height = info.Height;

        void* pixels{ info.Data ? info.Data : nullptr };
        b8 isFromFile{ !pixels && mUsage == ImageUsage::Image && std::filesystem::exists(info.File) };
        if (isFromFile) {
            i32 channels{ 0 };
            pixels = stbi_load(info.File.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
        }

        vk::Extent3D extent{};
        extent.width = width;
        extent.height = height;
        extent.depth = 1;

        vk::ImageCreateInfo createInfo{};
        createInfo.imageType = vk::ImageType::e2D;
        createInfo.extent = extent;
        createInfo.mipLevels = 1;
        createInfo.arrayLayers = 1;
        createInfo.tiling = vk::ImageTiling::eOptimal;
        createInfo.initialLayout = vk::ImageLayout::eUndefined;
        createInfo.sharingMode = vk::SharingMode::eExclusive;
        createInfo.samples = vk::SampleCountFlagBits::e1;

        if (info.Usage == ImageUsage::Image) {
            createInfo.format = vk::Format::eR8G8B8A8Srgb;
            createInfo.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
        }
        else if (info.Usage == ImageUsage::ColorAttachment) {
            createInfo.format = vk::Format::eR8G8B8A8Srgb;
            createInfo.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment;
        }
          else if (info.Usage == ImageUsage::DepthAttachment) {
            createInfo.format = vk::Format::eD32Sfloat;
            createInfo.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eDepthStencilAttachment;
        }

        mFormat = createInfo.format;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        vmaCreateImage(VkCore::Instance()->Allocator(), reinterpret_cast<VkImageCreateInfo*>(&createInfo), &allocInfo, reinterpret_cast<::VkImage*>(&mImage), &mAllocation, nullptr);

        if (info.Usage == ImageUsage::Image) {
            ChangeImageLayout(mImage, 1, createInfo.format, createInfo.initialLayout, vk::ImageLayout::eTransferDstOptimal, 1);
            
            VkBuffer stagingBuffer{ width * height * (u32)4, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_ONLY };
            stagingBuffer.InsertData(pixels);
              
            VkCore::Instance()->SubmitInstantCommand([&](vk::CommandBuffer cmd) {
                vk::BufferImageCopy region{};
                region.bufferOffset = 0;
                region.bufferRowLength = 0;
                region.bufferImageHeight = 0;
                region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
                region.imageSubresource.mipLevel = 0;
                region.imageSubresource.baseArrayLayer = 0;
                region.imageSubresource.layerCount = 1;
                region.imageOffset = vk::Offset3D{ 0, 0, 0 };
                region.imageExtent = extent;

                cmd.copyBufferToImage(stagingBuffer.Buffer(), mImage, vk::ImageLayout::eTransferDstOptimal, 1, &region);
            });
              
            ChangeImageLayout(mImage, 1, createInfo.format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, 1);
            if (isFromFile) {
                stbi_image_free(pixels);
            }

            mLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

            return;
        }
        else if (info.Usage == ImageUsage::ColorAttachment) {
            ChangeImageLayout(mImage, 1, createInfo.format, createInfo.initialLayout, vk::ImageLayout::eShaderReadOnlyOptimal, 1);
            mLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        }
        else if (info.Usage == ImageUsage::DepthAttachment) {
            ChangeImageLayout(mImage, 1, createInfo.format, createInfo.initialLayout, vk::ImageLayout::eDepthStencilAttachmentOptimal, 1);
            mLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        }
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

            if (info.Usage == ImageUsage::Image) {
                createInfo.format = vk::Format::eR8G8B8A8Srgb;
                createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            }
            else if (info.Usage == ImageUsage::ColorAttachment) {
                createInfo.format = vk::Format::eR8G8B8A8Srgb;
                createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            }
            else if (info.Usage == ImageUsage::DepthAttachment) {
                createInfo.format = vk::Format::eD32Sfloat;
                createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
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