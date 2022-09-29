#include "Buffers.h"
#include "Graphics/GraphicsCore.h"
#include "Platform/Vulkan/VkBuffers.h"

namespace Snow {
    VertexBuffer* VertexBuffer::Create(const Vertex* vertices, u64 count) {
        switch (Graphics::Api()) {
            case GraphicsAPI::Vulkan: return new VkVertexBuffer(vertices, count);
        }
        return nullptr;
    }

    IndexBuffer* IndexBuffer::Create(const u32* indices, u64 count) {
        switch (Graphics::Api()) {
            case GraphicsAPI::Vulkan: return new VkIndexBuffer(indices, count);
        }
        return nullptr;
    }
}