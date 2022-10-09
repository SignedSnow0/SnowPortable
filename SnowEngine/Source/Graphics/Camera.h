#pragma once
#include "Core/Common.h"

namespace Snow {
    enum class Projection : u32 {
        Perspective,
        Orthographic
    };

    class Camera {
    public:
        Camera() = default;
        ~Camera() = default;

        void SetPosition(const vec3f& position);
        void SetRotation(const vec3f& rotation);
        void SetAspectRatio(f32 aspectRatio);
        void SetFov(f32 fov);
        void SetNearPlane(f32 nearPlane);
        void SetFarPlane(f32 farPlane);
        void SetProjection(Projection projection);

        mat4f ViewMatrix() const;
        mat4f ProjectionMatrix() const;

    private:
        vec3f mPosition{ 0.0f, 0.0f, 0.0f };
        vec3f mFront{ 0.0f,0.0f,-1.0f };
        f32 mAspectRatio{ 16.0f / 9.0f };
        f32 mFOV{ 45.0f };
        f32 mNear{ 0.001f };
        f32 mFar{ 1000.0f };
        Projection mProjection{ Projection::Perspective };

        friend class SceneRenderer;//TODO: temporary
    };
}