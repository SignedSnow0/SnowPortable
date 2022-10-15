#pragma once 
#include "Core/Common.h"
#include "Graphics/Rhi/Image.h"
#include "Core/ResourcePtr.h"

namespace Snow {
    class DescriptorSet : public Resource<DescriptorSet> {
    public:
        virtual ~DescriptorSet() = default;

        virtual void SetUniform(const std::string& name, const void* data) = 0;
        virtual void SetImage(const std::string& name, Image* image) = 0;
        virtual void Bind() const = 0;

    protected:
        DescriptorSet() = default;
    };
}