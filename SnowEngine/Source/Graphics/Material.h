#pragma once
#include "Graphics/Rhi/Image.h"
#include "Graphics/Rhi/DescriptorSet.h"
#include "Graphics/Rhi/Pipeline.h"
#include "Core/ResourcePtr.h"

namespace Snow {
    class Material : public Resource<Material> {
    public:
        Material(Pipeline& pipeline);
        
        void Bind() const;

        void SetAlbedo(Image* albedo);

    private:
        ResourcePtr<Image> mAlbedo{ nullptr };

        ResourcePtr<DescriptorSet> mDescriptorSet{ nullptr };
    };
}