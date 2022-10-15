#pragma once
#include <filesystem>
#include <tiny_gltf.h>
#include "Core/Common.h"
#include "Mesh.h"
#include "Core/ResourcePtr.h"

namespace Snow {
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