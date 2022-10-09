#pragma once
#include <SnowEngine.h>

namespace ImGui {
    b8 TextImageButton(const std::string& label, ImTextureID image, const ImVec2& imageSize, b8 horizontal = false, const ImVec2& uv0 = ImVec2{ 0, 0 }, const ImVec2& uv1 = ImVec2{ 1, 1 }, i32 framePadding = { -1 });
    b8 Vec3fSlider(const std::string& label, vec3f& value, const vec3f& defaultValue = vec3f{ 0.0f });
}