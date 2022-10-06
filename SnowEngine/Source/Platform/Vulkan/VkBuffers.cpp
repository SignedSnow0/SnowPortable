#include "VkBuffers.h"
#include "VkCore.h"
#include "VkSurface.h"

namespace Snow {
    static void CopyBuffer(const vk::Buffer src, const vk::Buffer dst, const vk::DeviceSize size) {
        VkCore::Instance()->SubmitInstantCommand([&](vk::CommandBuffer cmd) {
            vk::BufferCopy copyRegion{};
            copyRegion.size = size;
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;
            
            cmd.copyBuffer(src, dst, copyRegion);
        });
    }
    
    VkBuffer::VkBuffer(u64 size, vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage)
        : mSize(size) {
        vk::BufferCreateInfo createInfo{};
        createInfo.size = size;
        createInfo.usage = usage;
        createInfo.sharingMode = vk::SharingMode::eExclusive;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = memoryUsage;

        vmaCreateBuffer(VkCore::Instance()->Allocator(), reinterpret_cast<VkBufferCreateInfo*>(&createInfo), &allocInfo, reinterpret_cast<::VkBuffer*>(&mBuffer), &mAllocation, nullptr);
    }

    VkBuffer::~VkBuffer() {
        vmaDestroyBuffer(VkCore::Instance()->Allocator(), mBuffer, mAllocation);
    }

    vk::Buffer VkBuffer::Buffer() const { return mBuffer; }

    void VkBuffer::InsertData(const void* data, u64 size, u64 offset) const {
        void* gpu;
        vmaMapMemory(VkCore::Instance()->Allocator(), mAllocation, &gpu);
        memcpy(reinterpret_cast<u8*>(gpu) + offset, data, size ? size : mSize);
        vmaUnmapMemory(VkCore::Instance()->Allocator(), mAllocation);
    }

    VkVertexBuffer::VkVertexBuffer(const Vertex* vertices, u64 count)
        : VkBuffer(sizeof(Vertex)* count, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, VMA_MEMORY_USAGE_GPU_ONLY), mCount{ count } {
        VkBuffer stagingBuffer(sizeof(Vertex) * count, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_ONLY);
        stagingBuffer.InsertData(vertices);

        CopyBuffer(stagingBuffer.Buffer(), mBuffer, mSize);
    }

    void VkVertexBuffer::Bind() const {
        VkSurface::BoundSurface()->CommandBuffer().bindVertexBuffers(0, mBuffer, { 0 });
    }

    void VkVertexBuffer::Draw() const {
        VkSurface::BoundSurface()->CommandBuffer().draw(mCount, 1, 0, 0);
    }
    
    vk::VertexInputBindingDescription VkVertexBuffer::BindingDescription() {
        vk::VertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = vk::VertexInputRate::eVertex;

        return bindingDescription;
    }

    std::vector<vk::VertexInputAttributeDescription> VkVertexBuffer::AttributeDescriptions() {
        std::vector<vk::VertexInputAttributeDescription> attributeDescriptions{};
        attributeDescriptions.resize(2);

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = vk::Format::eR32G32Sfloat;
        attributeDescriptions[0].offset = offsetof(Vertex, Position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[1].offset = offsetof(Vertex, Color);

        return attributeDescriptions;
    }

    VkIndexBuffer::VkIndexBuffer(const u32* indices, u64 count)
        : VkBuffer(sizeof(u32)* count, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, VMA_MEMORY_USAGE_GPU_ONLY), mCount{ count } {
        VkBuffer stagingBuffer(sizeof(u32) * count, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_ONLY);
        stagingBuffer.InsertData(indices);

        CopyBuffer(stagingBuffer.Buffer(), mBuffer, mSize);
    }

    void VkIndexBuffer::Bind() const {
        VkSurface::BoundSurface()->CommandBuffer().bindIndexBuffer(mBuffer, 0, vk::IndexType::eUint32);
    }

    void VkIndexBuffer::Draw() const {
        VkSurface::BoundSurface()->CommandBuffer().drawIndexed(mCount, 1, 0, 0, 0);
    }

    VkUniformBuffer::VkUniformBuffer(u64 size, u32 frameCount) {
        for (u32 i{ 0 }; i < frameCount; i++) {
            mBuffers.push_back(new VkBuffer(size, vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU));
        }
    }

    VkUniformBuffer::~VkUniformBuffer() {
        for (auto buffer : mBuffers) {
            delete buffer;
        }
    }

    const vk::Buffer VkUniformBuffer::Buffer(u32 frameIndex) const { return mBuffers[frameIndex]->Buffer(); }

    void VkUniformBuffer::SetData(const void* data, u32 frameIndex) const {
        mBuffers[frameIndex]->InsertData(data);
    }
}