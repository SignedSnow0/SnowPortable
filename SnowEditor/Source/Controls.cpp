#include "Controls.h"
#include <imgui_internal.h>

namespace ImGui {
    b8 TextImageButton(const std::string& text, ImTextureID image, const ImVec2& imageSize, b8 horizontal, const ImVec2& uv0, const ImVec2& uv1, i32 framePadding) {
		ImGuiWindow* window{ GetCurrentWindow() };
		if (window->SkipItems)
			return false;

		const ImGuiStyle& style = GImGui->Style;

		//get padding
		f32 innerSpacing = framePadding >= 0 ? (f32)framePadding : style.ItemInnerSpacing.y;
		ImVec2 padding = framePadding >= 0 ? ImVec2{ (f32)framePadding, (f32)framePadding } : style.FramePadding;

		const ImVec2 textSize = CalcTextSize(text.c_str(), nullptr, true);

		ImVec2 imgSize{ imageSize };
		imgSize.x += (padding.x * 2.0f);
		imgSize.y += (padding.y * 2.0f);

		//get size of image + text in correct orientation
		ImVec2 totalSize{ imgSize };
		horizontal ? totalSize.x += innerSpacing + textSize.x : totalSize.y += innerSpacing + textSize.y;

		//get cursor pos and sizes
		ImVec2 start{ window->DC.CursorPos };
		ImRect imageBB{ start, {start.x + imgSize.x, start.y + imgSize.y} };
		ImRect totalBB{ start, {start.x + totalSize.x, start.y + totalSize.y} };

		//add bounding box
		const ImGuiID id = window->GetID(text.c_str());
		ItemSize(totalBB);
		if (!ItemAdd(totalBB, id))
			return false;

		//add button behavior
		b8 hovered{ false };
		b8 held{ false };
		b8 pressed = ButtonBehavior(totalBB, id, &hovered, &held);

		//add text clipping
		PushClipRect(totalBB.Min, totalBB.Max, true);

		//render button background
		const ImU32 col = GetColorU32((hovered && held) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		RenderFrame(totalBB.Min, totalBB.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f, style.FrameRounding));

		//render image
		window->DrawList->AddImage(image, imageBB.Min, imageBB.Max, uv0, uv1, GetColorU32({ 1, 1, 1, 1 }));

		//render centered text in correct orientation
		if (horizontal) {
			start.x += imgSize.x;
			f32 dy = totalSize.y - textSize.y;
			if (dy <= padding.y * 2.0f)
				dy = padding.y * 2.0f;

			start.y += dy / 2.0f;

			RenderText(start, text.c_str());
		}
		else {
			start.y += imgSize.y - innerSpacing;

			f32 dx = totalSize.x - textSize.x;
			if (dx < padding.x * 2.0f)
				dx = padding.x * 2.0f;

			start.x += dx / 2.0f;

			RenderText(start, text.c_str());
		}

		PopClipRect();
		return pressed;
	}
}