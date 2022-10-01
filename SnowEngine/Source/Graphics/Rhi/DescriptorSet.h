#pragma once 
#include "Core/Common.h"

namespace Snow {
    class DescriptorSet {
    public:
        virtual ~DescriptorSet() = default;

        virtual void SetUniform(const std::string& name, const void* data) = 0;
        virtual void Bind() = 0;

    protected:
        DescriptorSet() = default;
    };
}