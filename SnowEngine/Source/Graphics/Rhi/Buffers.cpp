#include "Buffers.h"
#include "Core/Logger.h"
#include "Graphics/GraphicsCore.h"
#include "Platform/Vulkan/VkBuffers.h"

namespace Snow {
    VertexBuffer* VertexBuffer::Create(const Vertex* vertices, u64 count) {
        switch (Graphics::Api()) {
            case GraphicsAPI::Vulkan: return new VkVertexBuffer(vertices, count);
        }
        
        LOG_ERROR("Tried to create vertex buffer with invalid graphics API");
        return nullptr;
    }

    IndexBuffer* IndexBuffer::Create(const u32* indices, u64 count) {
        switch (Graphics::Api()) {
            case GraphicsAPI::Vulkan: return new VkIndexBuffer(indices, count);
        }
        
        LOG_ERROR("Tried to create index buffer with invalid graphics API");
        return nullptr;
    }
}