#include "Camera.h"

namespace Snow {
    void Camera::SetPosition(const vec3f& position) { mPosition = position; }
    
    void Camera::SetRotation(const vec3f& rotation) { mFront = rotation; }
    
    void Camera::SetAspectRatio(f32 aspectRatio) { mAspectRatio = aspectRatio; }
    
    void Camera::SetFov(f32 fov) { mFOV = fov; }
    
    void Camera::SetNearPlane(f32 nearPlane) { mNear = nearPlane; }
    
    void Camera::SetFarPlane(f32 farPlane) { mFar = farPlane; }

    void Camera::SetProjection(Projection projection) { mProjection = projection; }

    mat4f Camera::ViewMatrix() const { return glm::lookAt(mPosition, mPosition + mFront, vec3f(0.0f, 1.0f, 0.0f)); }
    
    mat4f Camera::ProjectionMatrix() const { return mProjection == Projection::Perspective ? glm::perspective(glm::radians(mFOV), mAspectRatio, mNear, mFar) : glm::ortho(-mAspectRatio, mAspectRatio, -1.0f, 1.0f, mNear, mFar); }
}