#include "Mesh.h"

namespace Snow {
    Mesh::~Mesh() {
        if (mVertexBuffer) {
			delete mVertexBuffer;
            delete mIndexBuffer;
        }
    }

    ResourcePtr<Material>& Mesh::GetMaterial() { return mMaterial; }
    
    void Mesh::Draw() const {
        if (mVertexBuffer) {
            mMaterial->Bind();

            mVertexBuffer->Bind();
            mIndexBuffer->Bind();
            mIndexBuffer->Draw();
        }
       
        for (const auto& child : mChildren) {
            child->Draw();
        }
    }
}