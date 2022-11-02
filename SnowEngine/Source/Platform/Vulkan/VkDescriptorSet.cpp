#include "VkDescriptorSet.h"
#include "VkCore.h"
#include "VkSurface.h"
#include "Core/Logger.h"

namespace Snow {
    VkDescriptorSet::VkDescriptorSet(const VkShaderLayout* layout, u32 frameCount, vk::PipelineLayout pLayout, u32 setLocation)
        : mLayout{ pLayout }, mSetLocation{ setLocation } {
        CreateDescriptorPool(layout, frameCount);
        CreateUniformBuffers(layout, frameCount);
    }

    VkDescriptorSet::~VkDescriptorSet() {
        VkCore::Instance()->Device().destroyDescriptorPool(mDescriptorPool);

        for (auto& [name, resource] : mResources) {
            if (resource.Type == ShaderResourceType::Uniform) {
                delete resource.Uniform;
            }
        }
    }

    void VkDescriptorSet::SetUniform(const std::string& name, const void* data) {
        const auto& resource{ mResources.find(name) };
        if (resource != mResources.end() && resource->second.Type == ShaderResourceType::Uniform) {
            resource->second.Uniform->SetData(data, VkSurface::BoundSurface()->CurrentFrame());
            return;
        }

        LOG_WARN("Trying to set uniform \"{}\" that does not exist", name);
    }

    void VkDescriptorSet::SetImage(const std::string& name, Image* image) {
        VkImage* vkImage{ reinterpret_cast<VkImage*>(image) };
        const auto& resource{ mResources.find(name) };
        if (resource != mResources.end() && resource->second.Type == ShaderResourceType::Image) {
            resource->second.Image = vkImage;

            vk::DescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = vkImage->Layout();
            imageInfo.imageView = vkImage->View();
            imageInfo.sampler = vkImage->Sampler();

            VkSurface::BoundSurface()->AddToDeletionQueue([resource, imageInfo, this](u32 frame) {
                vk::WriteDescriptorSet write{};
                write.dstSet = mDescriptorSets[frame];
                write.dstBinding = resource->second.Binding;
                write.dstArrayElement = 0;
                write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
                write.descriptorCount = 1;
                write.pImageInfo = &imageInfo;

                VkCore::Instance()->Device().updateDescriptorSets(write, nullptr);
            });
           
            return;
        }

        LOG_WARN("Trying to set image \"{}\" that does not exist", name);
    }

    void VkDescriptorSet::Bind() const {
        LOG_ASSERT(VkSurface::BoundSurface(), "Trying to bind descriptor set without a bound surface");
        VkSurface::BoundSurface()->CommandBuffer().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mLayout, mSetLocation, 1, &mDescriptorSets[VkSurface::BoundSurface()->CurrentFrame()], 0, nullptr);
    }

    void VkDescriptorSet::CreateDescriptorPool(const VkShaderLayout* layout, u32 frameCount) {
        std::vector<vk::DescriptorPoolSize> mSizes;

        u32 uniformCount{ 0 };
        u32 imageCount{ 0 };
        for (const auto& [binding, resource] : layout->Resources()) {
            if (resource->Type == ShaderResourceType::Uniform)
                uniformCount++;
            else if (resource->Type == ShaderResourceType::Image)
                imageCount++;
        }

        LOG_ASSERT(uniformCount || imageCount, "Trying to create descriptor set with no resources");

        if (uniformCount > 0)
            mSizes.push_back(vk::DescriptorPoolSize{ vk::DescriptorType::eUniformBuffer, uniformCount });
        if (imageCount > 0)
            mSizes.push_back(vk::DescriptorPoolSize{ vk::DescriptorType::eCombinedImageSampler, imageCount });
        
        vk::DescriptorPoolCreateInfo createInfo{};
        createInfo.poolSizeCount = static_cast<u32>(mSizes.size());
        createInfo.pPoolSizes = mSizes.data();
        createInfo.maxSets = frameCount;

        mDescriptorPool = VkCore::Instance()->Device().createDescriptorPool(createInfo);

        std::vector<vk::DescriptorSetLayout> layouts{ frameCount, layout->Layout() };
        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.descriptorPool = mDescriptorPool;
        allocInfo.descriptorSetCount = static_cast<u32>(layouts.size());
        allocInfo.pSetLayouts = layouts.data();

        mDescriptorSets = VkCore::Instance()->Device().allocateDescriptorSets(allocInfo);
    }

    void VkDescriptorSet::CreateUniformBuffers(const VkShaderLayout* layout, u32 frameCount) {
        for (const auto& [binding, description] : layout->Resources()) {
            VkShaderResource vkDescription{ *reinterpret_cast<const VkShaderResource*>(description) };
            if (vkDescription.Type == ShaderResourceType::Uniform) {
                mResources.insert({ vkDescription.Name, { vkDescription.Type, binding, new VkUniformBuffer(vkDescription.Size, frameCount) } });
            }
            else if (vkDescription.Type == ShaderResourceType::Image) {
                mResources.insert({ vkDescription.Name, { vkDescription.Type, binding, nullptr } });
            }
        }

        for (u32 i = 0; i < frameCount; i++) {
            std::vector<vk::WriteDescriptorSet> writes;
            for (const auto& [name, resource] : mResources) {
                if (resource.Type == ShaderResourceType::Uniform) {
                    vk::DescriptorBufferInfo bufferInfo{};
                    bufferInfo.buffer = resource.Uniform->Buffer(i);
                    bufferInfo.offset = 0;
                    bufferInfo.range = reinterpret_cast<VkShaderResource*>(layout->Resources().at(resource.Binding))->Size;

                    vk::WriteDescriptorSet write{};
                    write.dstSet = mDescriptorSets[i];
                    write.dstBinding = resource.Binding;
                    write.dstArrayElement = 0;
                    write.descriptorType = vk::DescriptorType::eUniformBuffer;
                    write.descriptorCount = 1;
                    write.pBufferInfo = &bufferInfo;

                    writes.push_back(write);
                }
            }

            VkCore::Instance()->Device().updateDescriptorSets(static_cast<u32>(writes.size()), writes.data(), 0, nullptr);
        }
    }
}