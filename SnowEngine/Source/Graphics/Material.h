#pragma once
#include "Graphics/Rhi/Image.h"
#include "Graphics/Rhi/DescriptorSet.h"
#include "Graphics/Rhi/Pipeline.h"
#include "Core/ResourcePtr.h"

namespace Snow {
    class Material : public Resource<Material> {
    public:
        Material(Pipeline& pipeline);
        
        void SetAlbedo(const ResourcePtr<Image>& albedo);
        ResourcePtr<Image> GetAlbedo() const;

        void Bind() const;


    private:
        ResourcePtr<Image> mAlbedo{ nullptr };

        ResourcePtr<DescriptorSet> mDescriptorSet{ nullptr };
    };
}