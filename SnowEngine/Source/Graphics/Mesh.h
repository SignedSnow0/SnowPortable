#pragma once 
#include "Core/Common.h"
#include "Graphics/Rhi/Buffers.h"
#include "Material.h"

namespace Snow {
    class Mesh : public Resource<Mesh> {
    public:
		~Mesh();

        ResourcePtr<Material>& GetMaterial();
        
        void Draw() const;
        
    private:
        std::vector<ResourcePtr<Mesh>> mChildren;
        mat4f mTransform;
        std::string mName;

        VertexBuffer* mVertexBuffer{ nullptr };
        IndexBuffer* mIndexBuffer{ nullptr };
        ResourcePtr<Material> mMaterial{ nullptr };

        friend class GltfImporter;
    };
}