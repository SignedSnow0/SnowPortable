#pragma once
#include "Common.h"
#include "Graphics/Mesh.h"

namespace Snow {
    struct TransformComponent {
        vec3f Position{ 0.0f, 0.0f, 0.0f };
        vec3f Rotation{ 0.0f, 0.0f, 0.0f };
        vec3f Scale{ 1.0f, 1.0f, 1.0f };

        mat4f Transform() const;
    };

    struct TagComponent {
        std::string Tag;
    };

    struct MeshComponent {
        Mesh* Mesh{ nullptr };
    };
}