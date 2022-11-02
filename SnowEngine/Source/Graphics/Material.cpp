#include "Material.h"

namespace Snow {
    Material::Material(Pipeline& pipeline)
        : mDescriptorSet{ pipeline.CreateDescriptorSet(1) } {
        
    }

    void Material::SetAlbedo(const ResourcePtr<Image>& albedo) {
        mAlbedo = albedo;
        mDescriptorSet->SetImage("albedo", mAlbedo);
    }
    
    ResourcePtr<Image> Material::GetAlbedo() const { return mAlbedo; }
    
    void Material::Bind() const {
        mDescriptorSet->Bind();
    }    
}