#include "Material.h"

namespace Snow {
    Material::Material(Pipeline& pipeline)
        : mDescriptorSet{ pipeline.CreateDescriptorSet(1) } {
        
    }

    void Material::Bind() const {
        mDescriptorSet->Bind();
    }

    void Material::SetAlbedo(Image* albedo) {
        mAlbedo = albedo;
        mDescriptorSet->SetImage("albedo", mAlbedo);
    }
}