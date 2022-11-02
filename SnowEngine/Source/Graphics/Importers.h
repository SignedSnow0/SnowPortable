#pragma once
#include <filesystem>
#include <tiny_gltf.h>
#include "Core/Common.h"
#include "Mesh.h"
#include "Core/ResourcePtr.h"
#include "Graphics/Rhi/Shader.h"

namespace Snow {
    class Importer {
    public:
        static ResourcePtr<Mesh> ImportMesh(const std::filesystem::path& path, b8 forceImport = false);
        static ResourcePtr<Shader> ImportShader(const std::vector<std::pair<std::filesystem::path, ShaderStage>>& shaders, const std::string& name = "Shader", b8 forceImport = false);

        static const std::map<u64, Shader*>& ShaderCache();
        
    private:
        static std::map<std::filesystem::path, Mesh*> sMeshCache;
        static std::map<u64, Shader*> sShaderCache;
    };

    class GltfImporter {
    public:
        Mesh* ImportModel(const std::filesystem::path& path);

    private:
        void PreloadImages(const tinygltf::Model& model);
        void PreloadMaterials(const tinygltf::Model& model);
        
        void ProcessNode(Mesh* mesh, const tinygltf::Node& node, const tinygltf::Model& model);
        void ProcessBuffers(Mesh* mesh, const tinygltf::Mesh& gltfMesh, const tinygltf::Model& model);

        std::vector<ResourcePtr<Image>> mLoadedImages;
        std::vector<ResourcePtr<Material>> mLoadedMaterials;
    };
}