#pragma once
#include "Graphics/Rhi/Image.h"
#include "Graphics/Rhi/DescriptorSet.h"
#include "Graphics/Rhi/Pipeline.h"
#include "Core/ResourcePtr.h"

namespace Snow {
    class Material : public Resource<Material> {
    public:
        Material(Pipeline& pipeline, const std::string& name = "Material");
        
        void SetAlbedo(const ResourcePtr<Image>& albedo);
        ResourcePtr<Image> GetAlbedo() const;
        void SetNormal(const ResourcePtr<Image>& normal);
        ResourcePtr<Image> GetNormal() const;
        void SetMetallicRoughness(const ResourcePtr<Image>& metallicRoughness);
        ResourcePtr<Image> GetMetallicRoughness() const;

        const std::string& Name() const;
        
        void Bind() const;

    private:
        ResourcePtr<Image> mAlbedo{ nullptr };
        ResourcePtr<Image> mNormal{ nullptr };
        ResourcePtr<Image> mMetallicRoughness{ nullptr };

        ResourcePtr<DescriptorSet> mDescriptorSet{ nullptr };

        std::string mName;
    };
}