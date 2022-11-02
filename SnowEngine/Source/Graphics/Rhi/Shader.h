#pragma once
#include <filesystem>
#include <map>
#include "Core/Common.h"
#include "Core/ResourcePtr.h"

namespace Snow {
    typedef u32 setLocation;
    typedef u32 bindingLocation;
    
    enum class ShaderStage : u32 {
        Vertex,
        Fragment,
        Compute
    };

    enum class ShaderResourceType : u32 {
        Uniform,
        Image
    };

    struct ShaderResource {
        std::string Name;
        u32 Size;
        ShaderResourceType Type;
    };

    struct ShaderPushConstant {
        std::string Name;
        u32 Size;
        u32 Offset;
    };

    struct ShaderLayout {
        virtual const std::map<u32, ShaderResource*>& Resources() const = 0;
    };


    class Shader : public Resource<Shader> {
    public:
        static Shader* Create(const std::vector<std::pair<std::filesystem::path, ShaderStage>>& shaders, const std::string& name = "Shader");
        virtual ~Shader() = default;

        virtual const std::map<setLocation, ShaderLayout*>& Layouts() const = 0;
        const std::string& Name() const;
        
        virtual void Reload() = 0;

    protected:
        Shader() = default;

    private:
        std::string mName{ "Shader" };
    };
}