#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Importers.h"
#include <glm/gtc/type_ptr.hpp>
#include "Core/Logger.h"
#include "Core/Components.h"
#include "SceneRenderer.h"
#include "Core/Utils.h"

namespace Snow {
    std::map<std::filesystem::path, Mesh*> Importer::sMeshCache{};
    std::map<u64, Shader*> Importer::sShaderCache{};

    ResourcePtr<Mesh> Importer::ImportMesh(const std::filesystem::path& path, b8 forceImport) {
        if (!forceImport && sMeshCache.find(path) != sMeshCache.end()) {
            return sMeshCache.at(path);
        }

        GltfImporter importer{};
        ResourcePtr<Mesh> mesh{ importer.ImportModel(path) };
        if (!forceImport) {        
            sMeshCache.insert({ path, mesh });
        }

        return mesh;
    }

    ResourcePtr<Shader> Importer::ImportShader(const std::vector<std::pair<std::filesystem::path, ShaderStage>>& shaders, const std::string& name, b8 forceImport) {
        u64 hash = 0;
        for (const auto& [path, stage] : shaders) {
            hash += std::hash<std::filesystem::path>{}(path);
        }

        if (!forceImport && sShaderCache.find(hash) != sShaderCache.end()) {
            return sShaderCache.at(hash);
        }

        ResourcePtr<Shader> shader{ Shader::Create(shaders, name) };
        if (!forceImport) {
            sShaderCache.insert({ hash, shader });
        }

        return shader;
    }    

    const std::map<u64, Shader*>& Importer::ShaderCache() { return sShaderCache; }
    
    //https://github.com/SaschaWillems/Vulkan-glTF-PBR/blob/master/base/VulkanglTFModel.cpp
    void GltfImporter::PreloadImages(const tinygltf::Model& model) {
        for (const auto& texture : model.textures) {
            tinygltf::Image image{ model.images[texture.source] };

            ImageCreateInfo info{};
            info.Usage = ImageUsage::Image;
            info.Width = image.width;
            info.Height = image.height;
            info.Data = image.image.data();
            info.File = image.uri;

            mLoadedImages.push_back(Image::Create(info));
        }
    }

    void GltfImporter::PreloadMaterials(const tinygltf::Model& model) {
        for (const auto& mat : model.materials) {
            Pipeline* pipeline{ SceneRenderer::DefaultPipeline() };
            ResourcePtr<Material> material{ new Material(*pipeline, mat.name) };
            ResourcePtr<Image> white{ Image::Create({ ImageUsage::Image, Utils::GetImagesPath() / "White.png" }) };

            if (mat.pbrMetallicRoughness.baseColorTexture.index >= 0) {
                material->SetAlbedo(mLoadedImages.at(mat.pbrMetallicRoughness.baseColorTexture.index));
            }
            else {
                material->SetAlbedo(white);
            }
            if (mat.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0) {
                material->SetMetallicRoughness(mLoadedImages.at(mat.pbrMetallicRoughness.metallicRoughnessTexture.index));
            }
            else {
                material->SetMetallicRoughness(white);
            }
            if (mat.normalTexture.index >= -1) {
                material->SetNormal(mLoadedImages.at(mat.normalTexture.index));
            }
            else {
                material->SetNormal(white);
            }
            //TODO ao, emissive

            mLoadedMaterials.push_back(material);
        }
    }

    Mesh* GltfImporter::ImportModel(const std::filesystem::path& path) {
        static tinygltf::TinyGLTF loader;
        tinygltf::Model model;

        std::string err;
        std::string warn;

        if (!loader.LoadASCIIFromFile(&model, &err, &warn, path.string())) {
            if (!err.empty()) {
                LOG_ERROR("Glft importer error: {}", err);
            }
            if (!warn.empty()) {
                LOG_WARN("Glft importer warning: {}", warn);
            }

            return nullptr;
        }

        const tinygltf::Scene& defaultScene = model.scenes[model.defaultScene >= 0 ? model.defaultScene : 0];

        PreloadImages(model);
        PreloadMaterials(model);

		Mesh* rootMesh = new Mesh();

        if (defaultScene.nodes.size() > 1) {
            for (const auto& nodeIndex : defaultScene.nodes) {
                if (model.nodes[nodeIndex].mesh >= 0) {
					Mesh* child = new Mesh();
                    ProcessNode(child, model.nodes[nodeIndex], model);
                    rootMesh->mChildren.push_back(child);
                }
            }
        }
        else {
			ProcessNode(rootMesh, model.nodes[0], model);
        }

        return rootMesh;
    }

    void GltfImporter::ProcessNode(Mesh* mesh, const tinygltf::Node& node, const tinygltf::Model& model) {
        mesh->mName = node.name;
        if (node.matrix.size() == 16) {
            mesh->mTransform = glm::make_mat4x4(node.matrix.data());
        }
        else {
            TransformComponent transform{};
            if (node.translation.size() == 3) {
                transform.Position = glm::make_vec3(node.translation.data());
            }
            if (node.rotation.size() == 4) {
                transform.Rotation = glm::make_vec3(node.rotation.data());
            }
            if (node.scale.size() == 3) {
                transform.Scale = glm::make_vec3(node.scale.data());
            }
            mesh->mTransform = transform.Transform();
        }

        ProcessBuffers(mesh, model.meshes[node.mesh], model);

        for (const auto& childIndex : node.children) {
			Mesh* childMesh = new Mesh();
            ProcessNode(childMesh, model.nodes[childIndex], model);
			mesh->mChildren.push_back(childMesh);
        }
    }

    void GltfImporter::ProcessBuffers(Mesh* mesh, const tinygltf::Mesh& gltfMesh, const tinygltf::Model& model) {
        u32 vertexCount{ 0 };
        u32 indexCount{ 0 };

        for (const auto& primitive : gltfMesh.primitives) {
            vertexCount += static_cast<u32>(model.accessors[primitive.attributes.at("POSITION")].count);

            if (primitive.indices >= 0) {
                indexCount += static_cast<u32>(model.accessors[primitive.indices].count);
            }
        }

        std::vector<Vertex> vertices{ vertexCount };
        std::vector<u32> indices{};
        indices.resize(indexCount);

        for (const auto& primitive : gltfMesh.primitives) {
            const f32* positions{ nullptr };
            i32 positionStride{ 0 };

            const f32* normals{ nullptr };
            i32 normalStride{ 0 };

            const f32* uvs{ nullptr };
            i32 uvStride{ 0 };

            const f32* colors{ nullptr };
            i32 colorStride{ 0 };

            const auto& posAccessor{ model.accessors[primitive.attributes.at("POSITION")] };
            const auto& posView{ model.bufferViews[posAccessor.bufferView] };
            positions = reinterpret_cast<const f32*>(&model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]);
            positionStride = posAccessor.ByteStride(posView) ? posAccessor.ByteStride(posView) / sizeof(f32) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);

            if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                const auto& normalAccessor{ model.accessors[primitive.attributes.at("NORMAL")] };
                const auto& normalView{ model.bufferViews[normalAccessor.bufferView] };
                normals = reinterpret_cast<const f32*>(&(model.buffers[normalView.buffer].data[normalAccessor.byteOffset + normalView.byteOffset]));
                normalStride = normalAccessor.ByteStride(normalView) ? normalAccessor.ByteStride(normalView) / sizeof(f32) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
            }

            if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
                const auto& uvAccessor{ model.accessors[primitive.attributes.at("TEXCOORD_0")] };
                const auto& uvView{ model.bufferViews[uvAccessor.bufferView] };
                uvs = reinterpret_cast<const f32*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                uvStride = uvAccessor.ByteStride(uvView) ? uvAccessor.ByteStride(uvView) / sizeof(f32) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
            }

            if (primitive.attributes.find("COLOR_0") != primitive.attributes.end()) {
                const auto& colorAccessor{ model.accessors[primitive.attributes.at("COLOR_0")] };
                const auto& colorView{ model.bufferViews[colorAccessor.bufferView] };
                colors = reinterpret_cast<const f32*>(&(model.buffers[colorView.buffer].data[colorAccessor.byteOffset + colorView.byteOffset]));
                colorStride = colorAccessor.ByteStride(colorView) ? colorAccessor.ByteStride(colorView) / sizeof(f32) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC4);
            }

            for (u64 i{ 0 }; i < posAccessor.count; i++) {
                Vertex& vertex{ vertices[i] };
                vertex.Position = glm::make_vec3(&positions[i * positionStride]);

                if (normals) {
                    vertex.Normal = glm::make_vec3(&normals[i * normalStride]);
                }
                if (uvs) {
                    vertex.UV = glm::make_vec2(&uvs[i * uvStride]);
                }
                if (colors) {
                    vertex.Color = glm::make_vec4(&colors[i * colorStride]);
                }
            }

            if (primitive.indices >= 0) {
                const auto& indexAccessor{ model.accessors[primitive.indices >= 0 ? primitive.indices : 0] };
                const auto& indexView{ model.bufferViews[indexAccessor.bufferView] };
                const void* indexBuffer = &model.buffers[indexView.buffer].data[indexAccessor.byteOffset + indexView.byteOffset];

                switch (indexAccessor.componentType) {
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
                        const u32* indicesPtr = reinterpret_cast<const u32*>(indexBuffer);
                        for (u64 i{ 0 }; i < indexAccessor.count; i++) {
                            indices[i] = indicesPtr[i];
                        }
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
                        const u16* indicesPtr = reinterpret_cast<const u16*>(indexBuffer);
                        for (u64 i{ 0 }; i < indexAccessor.count; i++) {
                            indices[i] = indicesPtr[i];
                        }
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
                        const u8* indicesPtr = reinterpret_cast<const u8*>(indexBuffer);
                        for (u64 i{ 0 }; i < indexAccessor.count; i++) {
                            indices[i] = indicesPtr[i];
                        }
                        break;
                    }
                }
            }
        }

        VertexBuffer* vb = VertexBuffer::Create(vertices.data(), vertices.size());
        IndexBuffer* ib = IndexBuffer::Create(indices.data(), indices.size());

        mesh->mMaterial = mLoadedMaterials[gltfMesh.primitives[0].material];
        mesh->mVertexBuffer = vb;
        mesh->mIndexBuffer = ib;
    }
}