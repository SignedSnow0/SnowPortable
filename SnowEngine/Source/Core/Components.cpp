#include "Components.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Snow {
    mat4f TransformComponent::Transform() const {
        return glm::translate(mat4f{ 1.0f }, Position) * glm::toMat4(quat{ Rotation }) * glm::scale(mat4f{ 1.0f }, Scale);
    }
}