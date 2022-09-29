#pragma once
#include "Core/Common.h"
#include "Graphics/Rhi/Buffers.h"
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

namespace Snow {
    class VkBuffer {
    public:
        VkBuffer(u64 size, vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage);
        virtual ~VkBuffer();

        vk::Buffer Buffer() const;

        void InsertData(const void* data, u64 size = 0, u64 offset = 0) const;

    protected:
        vk::Buffer mBuffer;
        VmaAllocation mAllocation;
        u64 mSize{ 0 };
    };

    class VkVertexBuffer : public VertexBuffer, public VkBuffer {
    public:
        VkVertexBuffer(const Vertex* vertices, u64 count);
        virtual ~VkVertexBuffer() override = default;

        virtual void Bind() const override;
        virtual void Draw() const override;

        static vk::VertexInputBindingDescription BindingDescription();
        static std::vector<vk::VertexInputAttributeDescription> AttributeDescriptions();

    private:
        u64 mCount{ 0 };
    };

    class VkIndexBuffer : public IndexBuffer, public VkBuffer {
    public:
        VkIndexBuffer(const u32* indices, u64 count);
        virtual ~VkIndexBuffer() override = default;
        
        virtual void Bind() const override;
        virtual void Draw() const override;

    private:
        u64 mCount{ 0 };
    };
}