#pragma once
#include "Core/Common.h"

namespace Snow {
    struct Vertex {
        vec3f Position;
        vec3f Color;
        vec2f UV;
        vec3f Normal;
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