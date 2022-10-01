#include "VkDescriptorSet.h"
#include "VkCore.h"
#include "VkSurface.h"

namespace Snow {
    VkDescriptorSet::VkDescriptorSet(const VkShaderLayout* layout, u32 frameCount, vk::PipelineLayout pLayout)
        : mLayout{ pLayout } {
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
        auto& resource = mResources[name];
        if (mResources.find(name) != mResources.end() && mResources.at(name).Type == ShaderResourceType::Uniform) {
            mResources.at(name).Uniform->SetData(data, VkSurface::BoundSurface()->CurrentFrame());
        }
    }

    void VkDescriptorSet::Bind() {
        VkSurface::BoundSurface()->CommandBuffer().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mLayout, 0, 1, &mDescriptorSets[VkSurface::BoundSurface()->CurrentFrame()], 0, nullptr);
    }

    void VkDescriptorSet::CreateDescriptorPool(const VkShaderLayout* layout, u32 frameCount) {
        std::vector<vk::DescriptorPoolSize> mSizes;

        u32 uniformCount{ 0 };
        u32 imageCount{ 0 };
        for (const auto& [binding, resource] : layout->Resources()) {
            if(resource.Type == ShaderResourceType::Uniform)
                uniformCount++;
            else if(resource.Type == ShaderResourceType::Image)
                imageCount++;
        }

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
            if (description.Type == ShaderResourceType::Uniform) {
                mResources.insert({ description.Name, { description.Type, binding, new VkUniformBuffer(description.Size, frameCount) } });
            }
        }

        for (u32 i = 0; i < frameCount; i++) {
            std::vector<vk::WriteDescriptorSet> writes;
            for (const auto& [name, resource] : mResources) {
                if (resource.Type == ShaderResourceType::Uniform) {
                    vk::DescriptorBufferInfo bufferInfo{};
                    bufferInfo.buffer = resource.Uniform->Buffer(i);
                    bufferInfo.offset = 0;
                    bufferInfo.range = layout->Resources().at(resource.Binding).Size;

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