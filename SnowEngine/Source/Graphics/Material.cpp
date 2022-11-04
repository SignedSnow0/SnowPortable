#include "Material.h"

namespace Snow {
    Material::Material(Pipeline& pipeline, const std::string& name)
        : mDescriptorSet{ pipeline.CreateDescriptorSet(1) }, mName{ name } {
        
    }

    void Material::SetAlbedo(const ResourcePtr<Image>& albedo) {
        mAlbedo = albedo;
        mDescriptorSet->SetImage("albedo", mAlbedo);
    }
    
    ResourcePtr<Image> Material::GetAlbedo() const { return mAlbedo; }

    void Material::SetNormal(const ResourcePtr<Image>& normal) {
        mNormal = normal;
        mDescriptorSet->SetImage("normalMap", mNormal);
    }

    ResourcePtr<Image> Material::GetNormal() const { return mNormal; }

    void Material::SetMetallicRoughness(const ResourcePtr<Image>& metallicRoughness) {
        mMetallicRoughness = metallicRoughness;
        mDescriptorSet->SetImage("metallicRoughness", mMetallicRoughness);
    }

    ResourcePtr<Image> Material::GetMetallicRoughness() const { return mMetallicRoughness; }

    const std::string& Material::Name() const { return mName; }
    
    void Material::Bind() const {
        mDescriptorSet->Bind();
    }    
}