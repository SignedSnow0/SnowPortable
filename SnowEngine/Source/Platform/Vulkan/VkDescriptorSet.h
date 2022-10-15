#pragma once 
#include <vulkan/vulkan.hpp>
#include "Graphics/Rhi/DescriptorSet.h"
#include "VkShader.h"
#include "VkBuffers.h"
#include "VkPipeline.h"
#include "VkImage.h"

namespace Snow {
    class VkDescriptorSet : public DescriptorSet {
    public:
        virtual ~VkDescriptorSet() override;

        virtual void SetUniform(const std::string& name, const void* data) override;
        virtual void SetImage(const std::string& name, Image* image) override;
        virtual void Bind() const override;

    private:
        struct ResourceBuffer {
            ShaderResourceType Type;
            u32 Binding;
            union {
                VkUniformBuffer* Uniform;
                VkImage* Image;
            };
        };
     
        VkDescriptorSet(const VkShaderLayout* layout, u32 frameCount, vk::PipelineLayout pLayout, u32 setLocation);
        void CreateDescriptorPool(const VkShaderLayout* layout, u32 frameCount);
        void CreateUniformBuffers(const VkShaderLayout* layout, u32 frameCount);

        vk::DescriptorPool mDescriptorPool;
        std::vector<vk::DescriptorSet> mDescriptorSets;
        vk::PipelineLayout mLayout;
        
        std::map<std::string, ResourceBuffer> mResources;
        setLocation mSetLocation;
        
        friend class VkPipeline;
    };
}