#pragma once
#include <SnowEngine.h>

namespace ImGui {
    b8 TextImageButton(const std::string& label, ImTextureID image, const ImVec2& imageSize, b8 horizontal = false, const ImVec2& uv0 = ImVec2{ 0, 0 }, const ImVec2& uv1 = ImVec2{ 1, 1 }, i32 framePadding = { -1 });
}