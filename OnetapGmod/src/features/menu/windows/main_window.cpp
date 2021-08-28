#include "main_window.h"

#include <imgui/im_tools.h>
#include "../widgets/widgets.h"

#include <fontawesomium.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include <d3d9.h>
#include <d3dx9.h>
#include "../../../globals.h"
#include "../../../settings/settings.h"
#include "../../logo.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#define IM_USE using namespace ImGui; 
using namespace ImGui;

class c_wittchen_tab_bar {
	int current_idx;
	ImVec2 overlay_pos;
	ImRect tabsrect;

public:
	bool begin();
	void end();

	bool tab(const std::string& name);
};

bool c_wittchen_tab_bar::begin() {
	return false;
}

void c_wittchen_tab_bar::end() {
	
}

bool TabButton(const std::string& name, int& cur_idx) {
	return false;
}
ImVec2 p;
ImDrawList* draw;
void shadow(ImDrawList* drawlist, ImVec2 pos, ImVec2 size, ImVec4 color, int rounding)
{
	while (true)
	{
		if (color.w <= 0.019f)
			break;
		drawlist->AddRect(pos, pos + size, ImGui::GetColorU32(color), rounding);
		color.w -= color.w / 8;
		pos -= ImVec2(1.f, 1.f);
		size += ImVec2(2.f, 2.f);
	}
}

bool checkbox(const char* label, bool* v)
{
	IM_USE;
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
	const float square_sz = ImGui::GetFrameHeight();
	const ImVec2 pos = window->DC.CursorPos;
	const ImRect total_bb(pos, pos + ImVec2(square_sz + (style.ItemInnerSpacing.x + label_size.x + 0), 10 + label_size.y + style.FramePadding.y * 2));
	ItemSize(total_bb, style.FramePadding.y);
	ItemAdd(total_bb, id);

	bool hovered, held;
	bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);

	if (hovered || held)
		ImGui::SetMouseCursor(0);

	if (pressed)
		*v = !(*v);

	window->DrawList->AddRectFilled(ImVec2(total_bb.Min.x + 0, total_bb.Min.y + 9), ImVec2(total_bb.Min.x + 13, total_bb.Min.y + 22), ImColor(175, 175, 179, 255), 3, 15);

	if (*v)
	{
		ImGui::RenderCheckMark(window->DrawList, ImVec2(total_bb.Min.x + 2, total_bb.Min.y + 11), ImColor(60, 60, 60), 9.f);
	}

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255 / 255.f, 254 / 255.f, 254 / 255.f, 255 / 255.f));
	ImGui::RenderText(ImVec2(total_bb.Min.x + style.ItemInnerSpacing.x + 15, total_bb.Min.y + style.FramePadding.y + 5), label);
	ImGui::PopStyleColor();
	return pressed;
}

bool button(const char* label, const ImVec2& size_arg)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, NULL);

	if (hovered || held)
		ImGui::SetMouseCursor(0);

	window->DrawList->AddRectFilled(ImVec2(bb.Min.x + 0, bb.Min.y + 0), ImVec2(bb.Max.x + 0, bb.Max.y + 0), ImColor(29, 28, 32, 244), 4, 15);
	window->DrawList->AddRect(ImVec2(bb.Min.x + 0, bb.Min.y + 0), ImVec2(bb.Max.x + 0, bb.Max.y + 0), ImColor(56, 54, 59, 255), 4, 15, 1.000000);
	window->DrawList->AddRect(ImVec2(bb.Min.x + -1, bb.Min.y + -1), ImVec2(bb.Max.x + 1, bb.Max.y + 1), ImColor(17, 17, 22, 255), 4, 15, 1.000000);
	window->DrawList->AddRect(ImVec2(bb.Min.x + 1, bb.Min.y + 1), ImVec2(bb.Max.x + -1, bb.Max.y + -1), ImColor(17, 17, 22, 255), 4, 15, 1.000000);

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(229 / 255.f, 229 / 255.f, 229 / 255.f, 255 / 255.f));
	ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
	ImGui::PopStyleColor();

	return pressed;
}

static ImVec2 CalcWindowContentSize(ImGuiWindow* window)
{
	if (window->Collapsed)
		if (window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
			return window->ContentSize;
	if (window->Hidden && window->HiddenFramesCannotSkipItems == 0 && window->HiddenFramesCanSkipItems > 0)
		return window->ContentSize;

	ImVec2 sz;
	sz.x = IM_FLOOR((window->ContentSizeExplicit.x != 0.0f) ? window->ContentSizeExplicit.x : window->DC.CursorMaxPos.x - window->DC.CursorStartPos.x);
	sz.y = IM_FLOOR((window->ContentSizeExplicit.y != 0.0f) ? window->ContentSizeExplicit.y : window->DC.CursorMaxPos.y - window->DC.CursorStartPos.y);
	return sz;
}

static ImVec2 CalcWindowSizeAfterConstraint(ImGuiWindow* window, ImVec2 new_size)
{
	ImGuiContext& g = *GImGui;
	if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint)
	{
		// Using -1,-1 on either X/Y axis to preserve the current size.
		ImRect cr = g.NextWindowData.SizeConstraintRect;
		new_size.x = (cr.Min.x >= 0 && cr.Max.x >= 0) ? ImClamp(new_size.x, cr.Min.x, cr.Max.x) : window->SizeFull.x;
		new_size.y = (cr.Min.y >= 0 && cr.Max.y >= 0) ? ImClamp(new_size.y, cr.Min.y, cr.Max.y) : window->SizeFull.y;
		if (g.NextWindowData.SizeCallback)
		{
			ImGuiSizeCallbackData data;
			data.UserData = g.NextWindowData.SizeCallbackUserData;
			data.Pos = window->Pos;
			data.CurrentSize = window->SizeFull;
			data.DesiredSize = new_size;
			g.NextWindowData.SizeCallback(&data);
			new_size = data.DesiredSize;
		}
		new_size.x = IM_FLOOR(new_size.x);
		new_size.y = IM_FLOOR(new_size.y);
	}

	// Minimum size
	if (!(window->Flags & (ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_AlwaysAutoResize)))
	{
		ImGuiWindow* window_for_height = window;
		new_size = ImMax(new_size, g.Style.WindowMinSize);
		new_size.y = ImMax(new_size.y, window_for_height->TitleBarHeight() + window_for_height->MenuBarHeight() + ImMax(0.0f, g.Style.WindowRounding - 1.0f)); // Reduce artifacts with very small windows
	}
	return new_size;
}

static ImVec2 CalcWindowAutoFitSize(ImGuiWindow* window, const ImVec2& size_contents)
{
	ImGuiContext& g = *GImGui;
	ImGuiStyle& style = g.Style;
	ImVec2 size_decorations = ImVec2(0.0f, window->TitleBarHeight() + window->MenuBarHeight());
	ImVec2 size_pad = window->WindowPadding * 2.0f;
	ImVec2 size_desired = size_contents + size_pad + size_decorations;
	if (window->Flags & ImGuiWindowFlags_Tooltip)
	{
		// Tooltip always resize
		return size_desired;
	}
	else
	{
		// Maximum window size is determined by the viewport size or monitor size
		const bool is_popup = (window->Flags & ImGuiWindowFlags_Popup) != 0;
		const bool is_menu = (window->Flags & ImGuiWindowFlags_ChildMenu) != 0;
		ImVec2 size_min = style.WindowMinSize;
		if (is_popup || is_menu) // Popups and menus bypass style.WindowMinSize by default, but we give then a non-zero minimum size to facilitate understanding problematic cases (e.g. empty popups)
			size_min = ImMin(size_min, ImVec2(4.0f, 4.0f));
		ImVec2 size_auto_fit = ImClamp(size_desired, size_min, ImMax(size_min, g.IO.DisplaySize - style.DisplaySafeAreaPadding * 2.0f));

		// When the window cannot fit all contents (either because of constraints, either because screen is too small),
		// we are growing the size on the other axis to compensate for expected scrollbar. FIXME: Might turn bigger than ViewportSize-WindowPadding.
		ImVec2 size_auto_fit_after_constraint = CalcWindowSizeAfterConstraint(window, size_auto_fit);
		bool will_have_scrollbar_x = (size_auto_fit_after_constraint.x - size_pad.x - size_decorations.x < size_contents.x && !(window->Flags& ImGuiWindowFlags_NoScrollbar) && (window->Flags& ImGuiWindowFlags_HorizontalScrollbar)) || (window->Flags & ImGuiWindowFlags_AlwaysHorizontalScrollbar);
		bool will_have_scrollbar_y = (size_auto_fit_after_constraint.y - size_pad.y - size_decorations.y < size_contents.y && !(window->Flags& ImGuiWindowFlags_NoScrollbar)) || (window->Flags & ImGuiWindowFlags_AlwaysVerticalScrollbar);
		if (will_have_scrollbar_x)
			size_auto_fit.y += style.ScrollbarSize;
		if (will_have_scrollbar_y)
			size_auto_fit.x += style.ScrollbarSize;
		return size_auto_fit;
	}
}
ImVec2 CalcWindowExpectedSize(ImGuiWindow* window)
{
	ImVec2 size_contents = CalcWindowContentSize(window);
	ImVec2 size_auto_fit = CalcWindowAutoFitSize(window, size_contents);
	ImVec2 size_final = CalcWindowSizeAfterConstraint(window, size_auto_fit);
	return size_final;
}
static float CalcMaxPopupHeightFromItemCount(int items_count)
{
	ImGuiContext& g = *GImGui;
	if (items_count <= 0)
		return FLT_MAX;
	return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
}

bool combo(const char* label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items = -1);
bool combo(const char* label, int* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items = -1);
bool combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int popup_max_height_in_items = -1);

bool begincombo(const char* label, const char* preview_value, ImGuiComboFlags flags)
{
	IM_USE;
	ImGuiContext& g = *GImGui;
	bool has_window_size_constraint = (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint) != 0;
	g.NextWindowData.Flags &= ~ImGuiNextWindowDataFlags_HasSizeConstraint;

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	const float arrow_size = (flags & ImGuiComboFlags_NoArrowButton) ? 0.0f : ImGui::GetFrameHeight();
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
	const float expected_w = 299;
	const float w = (flags & ImGuiComboFlags_NoPreview) ? arrow_size : expected_w;

	const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w + 0, 0 + style.FramePadding.y * 2.0f + 47));
	const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + 0 : 0.0f, 0.0f));

	ImGui::ItemSize(total_bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(frame_bb, id, &hovered, &held);
	bool popup_open = ImGui::IsPopupOpen(id, ImGuiPopupFlags_None);

	if (hovered || held)
		ImGui::SetMouseCursor(7);

	const float value_x2 = ImMax(frame_bb.Min.x, frame_bb.Max.x - arrow_size);

	window->DrawList->AddRectFilled(ImVec2(frame_bb.Min.x + 1, frame_bb.Min.y + 26), ImVec2(frame_bb.Max.x + 4, frame_bb.Max.y + -1), ImColor(0.115454f, 0.112457f, 0.127451f, 0.956863f), 4, 15);
	window->DrawList->AddRect(ImVec2(frame_bb.Min.x + 0, frame_bb.Min.y + 25), ImVec2(frame_bb.Max.x + 5, frame_bb.Max.y + 0), ImColor(0.219608f, 0.215686f, 0.235294f, 1.000000f), 4, 15, 1.000000);
	window->DrawList->AddRect(ImVec2(frame_bb.Min.x + -1, frame_bb.Min.y + 24), ImVec2(frame_bb.Max.x + 6, frame_bb.Max.y + 1), ImColor(0.070588f, 0.070588f, 0.090196f, 1.000000f), 4, 15, 1.000000);
	window->DrawList->AddRect(ImVec2(frame_bb.Min.x + 1, frame_bb.Min.y + 26), ImVec2(frame_bb.Max.x + 4, frame_bb.Max.y + -1), ImColor(0.070588f, 0.070588f, 0.090196f, 1.000000f), 4, 15, 1.000000);
	if (!(flags & ImGuiComboFlags_NoArrowButton))
	{
		ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);
		if (value_x2 + arrow_size - style.FramePadding.x <= frame_bb.Max.x)
			ImGui::RenderArrow(window->DrawList, ImVec2(value_x2 + style.FramePadding.y + -2, frame_bb.Min.y + style.FramePadding.y + 30), text_col, ImGuiDir_Down, 1);
	}

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255 / 255.f, 254 / 255.f, 254 / 255.f, 255 / 255.f));
	if (preview_value != NULL && !(flags & ImGuiComboFlags_NoPreview))
		ImGui::RenderText(frame_bb.Min + style.FramePadding + ImVec2(5, 29), preview_value);
	ImGui::PopStyleColor();

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255 / 255.f, 254 / 255.f, 254 / 255.f, 255 / 255.f));
	if (label_size.x > 0)
		ImGui::RenderText(ImVec2(frame_bb.Min.x + style.ItemInnerSpacing.x + -4, frame_bb.Min.y + style.FramePadding.y + 0), label);
	ImGui::PopStyleColor();

	if ((pressed || g.NavActivateId == id) && !popup_open)
	{
		if (window->DC.NavLayerCurrent == 0)
			window->NavLastIds[0] = id;
		ImGui::OpenPopupEx(id, ImGuiPopupFlags_None);
		popup_open = true;
	}

	if (!popup_open)
		return false;

	if ((flags & ImGuiComboFlags_HeightMask_) == 0)
		flags |= ImGuiComboFlags_HeightRegular;
	IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiComboFlags_HeightMask_));
	int popup_max_height_in_items = -1;
	if (flags & ImGuiComboFlags_HeightRegular)     popup_max_height_in_items = 8;
	else if (flags & ImGuiComboFlags_HeightSmall)  popup_max_height_in_items = 4;
	else if (flags & ImGuiComboFlags_HeightLarge)  popup_max_height_in_items = 20;
	ImGui::SetNextWindowSizeConstraints(ImVec2(w, 0.0f), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

	char name[16];
	ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size);

	if (ImGuiWindow* popup_window = ImGui::FindWindowByName(name))
		if (popup_window->WasActive)
		{
			ImVec2 size_expected = CalcWindowExpectedSize(popup_window);
			if (flags & ImGuiComboFlags_PopupAlignLeft)
				popup_window->AutoPosLastDirection = ImGuiDir_Left;
			ImRect r_outer = ImGui::GetWindowAllowedExtentRect(popup_window);
			ImVec2 pos = ImGui::FindBestWindowPosForPopupEx(frame_bb.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, frame_bb, ImGuiPopupPositionPolicy_ComboBox);
			ImGui::SetNextWindowPos(pos);
		}

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.FramePadding.x, style.WindowPadding.y));
	bool ret = ImGui::Begin(name, NULL, window_flags);
	ImGui::PopStyleVar();

	if (!ret)
	{
		ImGui::EndPopup();
		return false;
	}
	return true;
}

static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
	const char* const* items = (const char* const*)data;
	if (out_text)
		*out_text = items[idx];
	return true;
}

static bool Items_SingleStringGetter(void* data, int idx, const char** out_text)
{
	// FIXME-OPT: we could pre-compute the indices to fasten this. But only 1 active combo means the waste is limited.
	const char* items_separated_by_zeros = (const char*)data;
	int items_count = 0;
	const char* p = items_separated_by_zeros;
	while (*p)
	{
		if (idx == items_count)
			break;
		p += strlen(p) + 1;
		items_count++;
	}
	if (!*p)
		return false;
	if (out_text)
		*out_text = p;
	return true;
}

bool combo(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int popup_max_height_in_items)
{
	IM_USE;
	ImGuiContext& g = *GImGui;
	const char* preview_value = NULL;
	if (*current_item >= 0 && *current_item < items_count)
		items_getter(data, *current_item, &preview_value);
	if (popup_max_height_in_items != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
		SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));
	if (!begincombo(label, preview_value, ImGuiComboFlags_None))
		return false;
	bool value_changed = false;

	for (int i = 0; i < items_count; i++)
	{
		PushID((void*)(intptr_t)i);
		const bool item_selected = (i == *current_item);
		const char* item_text;

		if (!items_getter(data, i, &item_text))
			item_text = " * Unknown item * ";

		if (Selectable(item_text, item_selected))
		{
			value_changed = true;
			*current_item = i;
		}
		if (item_selected)
			SetItemDefaultFocus();
		PopID();
	}

	EndCombo();
	return value_changed;
}

bool combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items)
{
	const bool value_changed = combo(label, current_item, Items_ArrayGetter, (void*)items, items_count, height_in_items);
	return value_changed;
}

bool combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items)
{
	int items_count = 0;
	const char* p = items_separated_by_zeros;

	while (*p)
	{
		p += strlen(p) + 1;
		items_count++;
	}

	bool value_changed = combo(label, current_item, Items_SingleStringGetter, (void*)items_separated_by_zeros, items_count, height_in_items);
	return value_changed;
}

bool tab(const char* label, const char* icon, bool selected)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = ImGui::CalcItemSize({ label_size.x + style.FramePadding.x * 2.0f + 16,25 }, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, NULL);

	if (hovered || held)
		ImGui::SetMouseCursor(7);

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, selected ? 1.f : 0.8f));
	ImGui::RenderText(ImVec2(bb.Min.x + style.FramePadding.x, bb.Min.y + 7), icon);
	ImGui::RenderText(ImVec2(bb.Min.x + style.FramePadding.x + 20, bb.Min.y + 5), label);
	ImGui::PopStyleColor();

	return pressed;
}

bool tab1(const char* label, const char* icon, bool selected)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = ImGui::CalcItemSize({ label_size.x + style.FramePadding.x * 2.0f + 16,25 }, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, NULL);

	if (hovered || held)
		ImGui::SetMouseCursor(7);

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, selected ? 1.f : 0.8f));
	ImGui::RenderText(ImVec2(bb.Min.x + style.FramePadding.x + 20, bb.Min.y + 5), label);
	ImGui::PopStyleColor();

	return pressed;
}
bool subtab(const char* label, bool selected)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = ImGui::CalcItemSize({ label_size.x, 25 }, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, NULL);

	if (hovered || held)
		ImGui::SetMouseCursor(7);

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, selected ? 1.f : 0.8f));
	ImGui::RenderText(ImVec2(bb.Min.x + 2, bb.Min.y + (15 / 2 - label_size.y / 2)), label);
	ImGui::PopStyleColor();

	return pressed;
}

bool category(const char* label, bool selected)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = ImGui::CalcItemSize({ 51, 50 }, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, NULL);

	if (hovered || held)
		ImGui::SetMouseCursor(7);

	if (selected)
		window->DrawList->AddRectFilled(bb.Min, bb.Max, ImColor(15, 15, 20, 250));

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, selected ? 1.f : 0.8f));
	ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
	ImGui::PopStyleColor();

	return pressed;
}

static const char* PatchFormatStringFloatToInt(const char* fmt)
{
	if (fmt[0] == '%' && fmt[1] == '.' && fmt[2] == '0' && fmt[3] == 'f' && fmt[4] == 0)
		return "%d";

	const char* fmt_start = ImParseFormatFindStart(fmt);
	const char* fmt_end = ImParseFormatFindEnd(fmt_start);
	if (fmt_end > fmt_start && fmt_end[-1] == 'f')
	{
		if (fmt_start == fmt && fmt_end[0] == 0)
			return "%d";
		ImGuiContext& g = *GImGui;
		ImFormatString(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), "%.*s%%d%s", (int)(fmt_start - fmt), fmt, fmt_end);
		return g.TempBuffer;
	}
	return fmt;
}

bool SliderScalar11(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const float w = 300;

	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
	const ImRect frame_bb(window->DC.CursorPos + ImVec2(0, 18), window->DC.CursorPos + ImVec2(w + 3, label_size.y + 11 + (style.FramePadding.y * 2)));
	const ImRect total_bb(frame_bb.Min + ImVec2(0, -20), frame_bb.Max + ImVec2(0, 5) + ImVec2(style.ItemInnerSpacing.x + label_size.x, 0.0f));

	ImGui::ItemSize(total_bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
		return false;

	if (format == NULL)
		format = ImGui::DataTypeGetInfo(data_type)->PrintFmt;
	else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0)
		format = PatchFormatStringFloatToInt(format);
	const bool hovered = ImGui::ItemHoverable(frame_bb, id);
	const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
	bool temp_input_is_active = temp_input_allowed && ImGui::TempInputIsActive(id);
	if (!temp_input_is_active)
	{
		const bool focus_requested = temp_input_allowed && ImGui::FocusableItemRegister(window, id);
		const bool clicked = (hovered && g.IO.MouseClicked[0]);
		if (focus_requested || clicked || g.NavActivateId == id || g.NavInputId == id)
		{
			ImGui::SetActiveID(id, window);
			ImGui::SetFocusID(id, window);
			ImGui::FocusWindow(window);
			g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
		}
	}

	window->DrawList->AddRectFilled(ImVec2(frame_bb.Min.x + 0, frame_bb.Min.y + 0), ImVec2(frame_bb.Max.x + 0, frame_bb.Max.y + 0), ImColor(43, 42, 47, 255), 6, 15);
	window->DrawList->AddRect(ImVec2(frame_bb.Min.x + 0, frame_bb.Min.y + 0), ImVec2(frame_bb.Max.x + 0, frame_bb.Max.y + 0), ImColor(73, 73, 77, 160), 6, 15, 1.000000);


	ImRect grab_bb;
	const bool value_changed = ImGui::SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);

	grab_bb.Min = frame_bb.Min;
	grab_bb.Min += ImVec2(0, 0);
	grab_bb.Max += ImVec2(2, 2);

	window->DrawList->AddRectFilled(ImVec2(grab_bb.Min.x + 0, grab_bb.Min.y + 0), ImVec2(grab_bb.Max.x + 0, grab_bb.Max.y + 0), ImColor(249, 165, 22, 255), 6, 15);

	window->DrawList->AddRect(ImVec2(grab_bb.Max.x + -14, grab_bb.Max.y + -16), ImVec2(grab_bb.Max.x + 0, grab_bb.Max.y + 4), ImColor(94, 94, 94, 28), 6, 15, 1.000000);
	window->DrawList->AddRectFilled(ImVec2(grab_bb.Max.x + -14, grab_bb.Max.y + -16), ImVec2(grab_bb.Max.x + 0, grab_bb.Max.y + 4), ImColor(255, 254, 254, 28), 6, 15);
	window->DrawList->AddRectFilled(ImVec2(grab_bb.Max.x + -14, grab_bb.Max.y + -16), ImVec2(grab_bb.Max.x + 0, grab_bb.Max.y + 4), ImColor(255, 255, 255, 255), 6, 15);

	char value_buf[64];
	const char* value_buf_end = value_buf + ImGui::DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

	if (hovered)
		ImGui::SetMouseCursor(7);

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(229 / 255.f, 229 / 255.f, 229 / 255.f, 255 / 255.f));
	ImGui::RenderText(ImVec2(frame_bb.Max.x + -303, frame_bb.Min.y + style.FramePadding.y + -22), label);
	ImGui::PopStyleColor();

	return value_changed;
}

bool slider_float(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
	return SliderScalar11(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
}

bool slider_int(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
	return SliderScalar11(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
}


uint32_t key;
IDirect3DTexture9* tImage = nullptr;
void main_window::draw_main_window() {
	auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	auto aaa = render_system::get_device();
	if (tImage == nullptr)
		D3DXCreateTextureFromFileInMemoryEx(aaa, &NameArry, sizeof(NameArry), 1231, 1744, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &tImage);

	if (tImage == nullptr)D3DXCreateTextureFromFileInMemoryEx(aaa
		, &NameArry, sizeof(NameArry),
		130, 30, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &tImage);
	ImGui::SetNextWindowSize({ 545.000000f,470.000000f });

	ImGui::Begin("Edited", nullptr, flags);
	{
		static bool bools[128];
		static int ints[128];
		static float floats[128];
		static int selectedtab = 1;
		static int selectedsubtab = 0;
		static int selectedcategory = 0;
		p = ImGui::GetWindowPos();
		draw = ImGui::GetWindowDrawList();
		draw->AddRectFilled(ImVec2(p.x + 0, p.y + 0), ImVec2(p.x + 545, p.y + 12), ImColor(249, 165, 22, 255), 6, 15);
		draw->AddRectFilled(ImVec2(p.x + 0, p.y + 460), ImVec2(p.x + 545, p.y + 470), ImColor(33, 33, 38, 255), 12, 15);
		draw->AddRectFilled(ImVec2(p.x + 0, p.y + 7), ImVec2(p.x + 545, p.y + 464), ImColor(33, 33, 38, 255), 0, 15);
			draw->AddLine(ImVec2(p.x + 0, p.y + 72), ImVec2(p.x + 545, p.y + 72), ImColor(89, 87, 93, 255), 1.000000);
			draw->AddLine(ImVec2(p.x + 0, p.y + 71), ImVec2(p.x + 545, p.y + 71), ImColor(22, 21, 26, 255), 1.000000);
			draw->AddLine(ImVec2(p.x + 0, p.y + 73), ImVec2(p.x + 545, p.y + 73), ImColor(22, 21, 26, 255), 1.000000);
			draw->AddLine(ImVec2(p.x + 140, p.y + 72), ImVec2(p.x + 140, p.y + 470), ImColor(79, 77, 82, 255), 1.000000);
			draw->AddLine(ImVec2(p.x + 192, p.y + 72), ImVec2(p.x + 192, p.y + 470), ImColor(79, 77, 82, 255), 1.000000);
			draw->AddLine(ImVec2(p.x + 191, p.y + 73), ImVec2(p.x + 191, p.y + 470), ImColor(22, 21, 26, 255), 1.000000);
			draw->AddLine(ImVec2(p.x + 193, p.y + 73), ImVec2(p.x + 193, p.y + 470), ImColor(22, 21, 26, 255), 1.000000);
			draw->AddLine(ImVec2(p.x + 139, p.y + 73), ImVec2(p.x + 139, p.y + 470), ImColor(22, 21, 26, 255), 1.000000);
			draw->AddLine(ImVec2(p.x + 141, p.y + 73), ImVec2(p.x + 141, p.y + 470), ImColor(22, 21, 26, 255), 1.000000);
		
		ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(p.x + 142, p.y + 73), ImVec2(p.x + 191, p.y + 470), ImColor(22, 21, 26, 255), 0, 15);

		
		ImGui::SetCursorPos({ (float)2, (float)20 });
		ImGui::Image(tImage, ImVec2(130, 30));
		ImGui::SetCursorPos({ (float)100, (float)26 });
		ImGui::BeginGroup();
		{
			if (tab("Aimbot", ICON_FA_CROSSHAIRS, selectedtab == 1))
				selectedtab = 1;
			ImGui::SameLine();
			if (tab("Visuals", ICON_FA_ADJUST, selectedtab == 2))
				selectedtab = 2;
			ImGui::SameLine();
			if (tab("Misc", ICON_FA_COG, selectedtab == 3))
				selectedtab = 3;
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(285,26));
			if (tab("Credits", "", selectedtab == 3))
				selectedtab = 4;		
		}
		ImGui::EndGroup();

		ImGui::SetCursorPos({ (float)141, (float)73 });
		ImGui::BeginGroup(); 
		{
			if (selectedtab==1) {

				if (category(ICON_FA_CROSSHAIRS, selectedcategory == 0))
					selectedcategory = 0;
				if (category(ICON_FA_EYE, selectedcategory == 1))
					selectedcategory = 1;
			}
			if (selectedtab == 2) {

				if (category(ICON_FA_EYE, selectedcategory == 0))
					selectedcategory = 0;
			}
			if (selectedtab == 3) {


				if (category(ICON_FA_COGS, selectedcategory == 0))
					selectedcategory = 0;
			}
		}
		ImGui::EndGroup();

		ImGui::SetCursorPos({ (float)27, (float)93 });
		ImGui::BeginGroup();
		{
			if (selectedtab == 1) {
				if (subtab("General", selectedsubtab == 0))
					selectedsubtab = 0;
			}
			if (selectedtab == 2) {
				if (subtab("General", selectedsubtab == 0))
					selectedsubtab = 0;
			}
			if (selectedtab == 3) {
				if (subtab("General", selectedsubtab == 0))
					selectedsubtab = 0;
				if (subtab("Lua", selectedsubtab == 2))
					selectedsubtab = 2;
			}
		}
		ImGui::EndGroup();

		ImGui::SetCursorPos({ (float)217, (float)96 });
		ImGui::BeginGroup();
		{
			/*if (begincombo("Combo", "Preview", NULL))
				ImGui::Selectable("Preview", true),
				ImGui::EndCombo();*/
			if (selectedtab == 1) {
				if (selectedsubtab == 0 && selectedcategory == 0) { //general
					checkbox("Enable", &settings::aimbotenable); // aimbot_autofire norecoil nospread aimbot_fov_draw aimbot_silent aimbot_draw_target
					checkbox("AutoFire", &settings::get_bool("aimbot_autofire"));
					checkbox("NoRecoil", &settings::get_bool("norecoil"));
					checkbox("NoSpread", &settings::get_bool("nospread"));
					checkbox("Silent", &settings::get_bool("aimbot_silent"));
					checkbox("Force accuracy fire", &settings::get_bool("faf"));
					int bones = settings::get_int("aimbot_bones");
					if (begincombo("Bone", "Bones", NULL)) {
						auto& bones = settings::get_int("aimbot_bones");
						for (auto i = 1; i <= (int)e_bones::last; ++i) {
							std::string str = to_string((e_bones)i);
							if (str.empty()) continue;
							if (Selectable(std::string(str + "##AIMBONE").c_str(), (bones & i), ImGuiSelectableFlags_DontClosePopups)) {
								if (!(bones & i))
									bones |= i;
								else
									bones &= ~i;
							}
							if (bones & i) SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
					slider_int("FOV", &settings::get_int("aimbot_fov"), 0, 180, NULL, NULL);
					Hotkey("Aimbot Key", &settings::aimbotkey);
				}
				else if (selectedsubtab == 0 && selectedcategory == 1)
				{
					checkbox("AimBot Fov Draw", &settings::get_bool("aimbot_fov_draw"));
					checkbox("Aimbot Draw Target", &settings::get_bool("aimbot_draw_target"));
				}
			}
			else if(selectedtab==2)
			{
				static Wittchen::WitthcenEspStyleEditor g_style_editor;
				static const char* text[]{ "filled", "border", "corner" };
				checkbox("Esp Enable", &settings::get_bool("esp_enable"));
				checkbox("Players Info", &settings::get_bool("esp_info"));
				combo("Box Type", &settings::get_int("esp_type"), text, IM_ARRAYSIZE(text));
			}
			else if (selectedtab == 3)
			{
				if (selectedsubtab == 0) {
					checkbox("Bunnyhop", &settings::get_bool("bhop"));
					checkbox("AutoStrafe", &settings::get_bool("autostrafe"));
					checkbox("FixMovement", &settings::get_bool("fixmovement"));
					checkbox("Third Person", &settings::get_bool("third_person"));
					slider_int("Custom Viewmodel Fov", &settings::get_int("custom_viewmodel_fov"), 0, 180, NULL, NULL);
					slider_int("Custom Fov", &settings::get_int("custom_fov"), 0, 180, NULL, NULL);
					slider_int("Custom Aspect Ratio", &settings::get_int("custom_aspect_ratio"), 0, 180, NULL, NULL);
					slider_int("Third Person Distance", &settings::get_int("third_person_distance"), 0, 180, NULL, NULL);
				}
				else if (selectedsubtab == 2)
				{
					if (button("Glua", { (float)303, (float)25 }))
						settings::glua = !settings::glua;
				}
			}
			else if (selectedtab == 4)
			{
				ImGui::Text("t.me/urbanichka");
				ImGui::Text("KIRAT23 & voidptr_t");
			}
		}
		ImGui::EndGroup();
	}
	ImGui::End();
}
