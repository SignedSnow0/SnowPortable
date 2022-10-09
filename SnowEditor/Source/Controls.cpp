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

	b8 Vec3fSlider(const std::string& label, vec3f& value, const vec3f& defaultValue) {
		b8 changed{ false };
		f32 speed{ 0.1f };
		/*if (IsKeyDown(GLFW_KEY_LEFT_SHIFT))
			speed = 0.01f;
		else if (IsKeyDown(GLFW_KEY_LEFT_CONTROL))
			speed = 1.0f;*/
		PushID(label.c_str());
		Columns(2, nullptr, false);
		
		//Label
		Text(label.c_str());
		NextColumn();

		//Values
		PushMultiItemsWidths(3, CalcItemWidth());
		PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

		f32 lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize{ lineHeight + 3.0f, lineHeight };

		//X
		PushStyleColor(ImGuiCol_Button, { 0.7f, 0.05f, 0.12f, 1.0f });
		PushStyleColor(ImGuiCol_ButtonHovered, { 0.8f, 0.15f, 0.22f, 1.0f });
		PushStyleColor(ImGuiCol_ButtonActive, { 0.7f, 0.05f, 0.12f, 1.0f });
		if (Button("X", buttonSize) && value.x != defaultValue.x) {
			value.x = defaultValue.x;
			changed = true;
		}
		PopStyleColor(3);
		SameLine();

		changed |= DragFloat("##X", &value.x, 0.1f, 0.0f, 0.0f, "%.3f");
		PopItemWidth();
		SameLine();

		//Y
		PushStyleColor(ImGuiCol_Button, { 0.26f, 0.63f, 0.26f, 1.0f });
		PushStyleColor(ImGuiCol_ButtonHovered, { 0.36f, 0.73f, 0.36f, 1.0f });
		PushStyleColor(ImGuiCol_ButtonActive, { 0.26f, 0.63f, 0.26f, 1.0f });
		if (Button("Y", buttonSize) && value.y != defaultValue.y) {
			value.y = defaultValue.y;
			changed = true;
		}
		PopStyleColor(3);
		SameLine();
		
		changed |= DragFloat("##Y", &value.y, 0.1f, 0.0f, 0.0f, "%.3f");
		PopItemWidth();
		SameLine();

		//Z
		PushStyleColor(ImGuiCol_Button, { 0.0f, 0.27f, 0.5f, 1.0f });
		PushStyleColor(ImGuiCol_ButtonHovered, { 0.1f, 0.37f, 0.6f, 1.0f });
		PushStyleColor(ImGuiCol_ButtonActive, { 0.0f, 0.27f, 0.5f, 1.0f });
		if (Button("Z", buttonSize) && value.z != defaultValue.z) {
			value.z = defaultValue.z;
			changed = true;
		}
		PopStyleColor(3);
		SameLine();
		
		changed |= DragFloat("##Z", &value.z, 0.1f, 0.0f, 0.0f, "%.3f");
		PopItemWidth();

		PopStyleVar();
		Columns(1);
		
		PopID();

		return true;
	}
}