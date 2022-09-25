#pragma once
#include <filesystem>
#include "Core/Common.h"

namespace Snow {
    enum class ShaderStage : u32 {
        Vertex,
        Fragment,
        Compute
    };
    
    class Shader {
    public:
        static Shader* Create(const std::vector<std::pair<std::filesystem::path, ShaderStage>>& shaders);
        virtual ~Shader() = default;

    protected:
        Shader() = default;
    };
}