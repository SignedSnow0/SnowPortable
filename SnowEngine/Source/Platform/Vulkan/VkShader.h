#pragma once
#include <map>
#include <vulkan/vulkan.hpp>
#include "Graphics/Rhi/Shader.h"

namespace Snow {
    typedef u32 setLocation;
    typedef u32 bindingLocation;

    enum class ShaderResourceType : u32 {
        Uniform,
        Image
    };

    struct VkShaderResource {
        std::string Name;
        u32 Size;
        vk::DescriptorSetLayoutBinding Description;
        ShaderResourceType Type;
    };

    struct VkShaderLayout {
        VkShaderLayout(const std::map<bindingLocation, VkShaderResource>& resources);
        ~VkShaderLayout();
        
        vk::DescriptorSetLayout Layout() const;
        const std::map<bindingLocation, VkShaderResource>& Resources() const;

    private:
        vk::DescriptorSetLayout mLayout;
        std::map<bindingLocation, VkShaderResource> mResources;
    };

    class VkShader : public Shader {
    public:
        VkShader(const std::vector<std::pair<std::filesystem::path, ShaderStage>>& shaders);
        virtual ~VkShader() override;

        std::vector<vk::PipelineShaderStageCreateInfo> ShaderStages() const;
        VkShaderLayout* Layout(setLocation set) const;
        std::vector<vk::DescriptorSetLayout> DescriptorLayouts() const;

    private:
        std::vector<u32> CompileShader(const std::filesystem::path& file, vk::ShaderStageFlagBits stage);
        void ReflectShader(const std::vector<u32>& shader, vk::ShaderStageFlagBits stage, std::map<setLocation, std::map<bindingLocation, VkShaderResource>>& reflection);
        
        std::map<vk::ShaderStageFlagBits, vk::ShaderModule> mShaderModules;
        std::map<setLocation, VkShaderLayout*> mLayouts;
    };
}