#pragma once
#include "Core/Common.h"

namespace Snow {
    struct Vertex {
        vec2f Position;
        vec3f Color;
    };
    
    class VertexBuffer {
    public:
        static VertexBuffer* Create(const Vertex* vertices, u64 count);
        virtual ~VertexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void Draw() const = 0;
    };

    class IndexBuffer {
    public:
        static IndexBuffer* Create(const u32* indices, u64 count);
        virtual ~IndexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void Draw() const = 0;
    };
}