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
#include <filesystem>
#include <fstream>
#include "../../lua_futures/lua_futures.h"
#include <file_tools.h>
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
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 255 / 255.f));
	RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));
	ImGui::PopStyleColor();
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
auto entity_lists_update_time_stamp = 0.f;
bool is_entlists_updating = false;
globals::c_entity_list ent_list;
// <steam_id, name>
std::map<std::string, std::string> players_list;

// <id, name>
struct team_t
{
	std::string name;
	c_color color;
};
std::map<int, team_t> teams_list;

inline void set_tooltip(const std::string& text, ...)
{
	if (ImGui::IsItemHovered())
	{
		auto fmt = text.c_str();
		ImGui::BeginTooltip();
		va_list args;
		va_start(args, fmt);
		ImGui::TextV(fmt, args);
		va_end(args);
		ImGui::EndTooltip();
	}
}

void main_window::update_entity_list()
{
	//Update every 5 second
	auto is_update = [&]()
	{
		if (entity_lists_update_time_stamp == 0.f)
			return true;

		const auto current_time_stamp = interfaces::engine->get_time_stamp_from_start();

		if (roundf(current_time_stamp) - roundf(entity_lists_update_time_stamp) < 5)
			return false;

		return true;
	};

	if (!is_update())
		return;

	//just guard
	is_entlists_updating = true;

	entity_lists_update_time_stamp = interfaces::engine->get_time_stamp_from_start();

	if (!interfaces::engine->is_in_game())
		return;

	//Clear lists
	ent_list.clear();
	players_list.clear();
	teams_list.clear();

	//Add entitys to entity list
	for (auto i = 0; i < interfaces::entity_list->get_highest_entity_index(); ++i)
	{
		auto* ent = get_entity_by_index(i);
		if (!ent)
			continue;

		auto class_name = ent->get_classname();

		if (ent_list.exist(class_name))
			continue;

		if (class_name.empty())
			continue;

		if (class_name.find("class ") != std::string::npos || class_name == "player" || class_name == "worldspawn")
			continue;



		ent_list.push_back(class_name);
	}

	//Add player to player list and jobs to teams_list
	for (auto i = 0; i < interfaces::entity_list->get_highest_entity_index(); ++i)
	{
		auto ply = get_player_by_index(i);
		if (!ply || !ply->is_player())
			continue;
		auto sid = ply->get_steam_id();

		if (sid.empty() || players_list.find(sid) != players_list.end())
			continue;

		players_list.emplace(sid, ply->get_name());

		if (teams_list.find(ply->get_team_num()) != teams_list.end())
			continue;

		team_t tmp{ ply->get_team_name(), ply->get_team_color() };
		teams_list.emplace(ply->get_team_num(), tmp);
	}

	is_entlists_updating = false;
}
bool drawentlist;

void draw_entity_list()
{
	using namespace ImGui;
	if (drawentlist) {
		int w, h;
		interfaces::engine->get_screen_size(w, h);
		ImGui::SetNextWindowSize({ w / 2.f, h / 2.f }, ImGuiCond_FirstUseEver);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(33 / 255.f, 33 / 255.f, 38 / 255.f, 180 / 255.f));

		Begin("Target list##SUBWINDOW");

		BeginTabBar("##ENTITY_LIST_TAB_BAR");

		if (BeginTabItem("Entities"))
		{
			static ImGuiTextFilter entity_filter;

			PushItemWidth(GetContentRegionAvailWidth() - (GetContentRegionAvailWidth() / 2.0f));
			entity_filter.Draw("Filter (inc,-exc)");
			PopItemWidth();
			SameLine();
			
			Hotkey("Add##ADD_ENTITY_HOTKEY", &settings::get_var<uint32_t>("add_entity_bind"), { GetContentRegionAvailWidth() / 1.5f, 0 });
			if (IsItemHovered())
			{
				BeginTooltip();
				Text("Add the entity you are looking at");
				EndTooltip();
			}

			if (ImGui::BeginTable("entities_table", 2, ImGuiTableFlags_BordersInner | ImGuiTableFlags_BordersOuter))
			{
				ImGui::TableSetupColumn("Name");
				ImGui::TableSetupColumn("ESP");
				ImGui::TableHeadersRow();

				if (!is_entlists_updating)
				{
					for (auto class_name : ent_list.data())
					{
						if (!entity_filter.PassFilter(class_name.c_str()))
							continue;

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::Text("%s", class_name.c_str());
						ImGui::TableNextColumn();
						if (button(globals::entitys_to_draw.exist(class_name) ? (std::string("Remove##ENTS_TABLE") + class_name).c_str() : (std::string("Add##ENTS_TABLE") + class_name).c_str(), { 0,0 }))
						{
							if (globals::entitys_to_draw.exist(class_name))
								globals::entitys_to_draw.remove(globals::entitys_to_draw.find(class_name));
							else
								globals::entitys_to_draw.push_back(class_name);
						}

					}
				}
				ImGui::EndTable();
			}

			EndTabItem();
		}
		if (BeginTabItem("Players"))
		{
			static ImGuiTextFilter player_filter;

			PushItemWidth(GetContentRegionAvailWidth() - (GetContentRegionAvailWidth() / 2.3f));
			player_filter.Draw("Filter (inc,-exc)");
			PopItemWidth();
			SameLine();
			Hotkey("Add##ADD_ENTITY_HOTKEY", &globals::entitykey, { GetContentRegionAvailWidth() / 1.5f, 0 });
			if (IsItemHovered())
			{
				BeginTooltip();
				Text("Add the entity you are looking at");
				EndTooltip();
			}

			if (BeginTable("players_table", 3, ImGuiTableFlags_BordersInner | ImGuiTableFlags_BordersOuter))
			{
				TableSetupColumn("Name");
				TableSetupColumn("STEAM-ID");
				TableSetupColumn("FRIEND");
				TableHeadersRow();

				if (!is_entlists_updating)
				{
					for (auto [steam_id, name] : players_list)
					{
						if (player_filter.PassFilter(steam_id.c_str()) || player_filter.PassFilter(name.c_str()))
						{
							TableNextRow();
							TableNextColumn();
							Text(name.c_str());
							TableNextColumn();
							Text(steam_id.c_str());
							TableNextColumn();


							if (button((std::find(globals::friends.begin(), globals::friends.end(), steam_id.c_str()) == globals::friends.end()) ?
								("Add##" + steam_id).c_str() :
								("Remove##" + steam_id).c_str(), {0,0}))
							{
								if (std::find(globals::friends.begin(), globals::friends.end(), steam_id.c_str()) == globals::friends.end())
									globals::friends.push_back(steam_id);
								else
									globals::friends.erase(std::find(globals::friends.begin(), globals::friends.end(), steam_id.c_str()));
							}
						}
					}
				}
				EndTable();
			}

			EndTabItem();
		}

		if (BeginTabItem("Teams"))
		{
			if (BeginTable("teams_table", 2, ImGuiTableFlags_BordersInner | ImGuiTableFlags_BordersOuter))
			{
				TableSetupColumn("Name");
				TableSetupColumn("Friendly");
				TableHeadersRow();

				if (!is_entlists_updating)
				{
					for (auto [id, team] : teams_list)
					{
						TableNextRow();
						TableNextColumn();
						TextColored(team.color.get_vec4(), team.name.c_str());
						TableNextColumn();
						if (button((std::find(globals::friendly_teams.begin(), globals::friendly_teams.end(), id) == globals::friendly_teams.end()) ?
							("Add##TEAM_" + std::to_string(id)).c_str() :
							("Remove##TEAM_" + std::to_string(id)).c_str(), {0,0}))
						{
							if (std::find(globals::friendly_teams.begin(), globals::friendly_teams.end(), id) == globals::friendly_teams.end())
								globals::friendly_teams.push_back(id);
							else
								globals::friendly_teams.erase(std::find(globals::friendly_teams.begin(), globals::friendly_teams.end(), id));
						}
					}
				}

				EndTable();
			}

			EndTabItem();
		}
		ImGui::PopStyleColor();
		EndTabBar();
		End();
	}
}
bool ColorEdit44(const char* label, float col[4], ImGuiColorEditFlags flags)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const float square_sz = GetFrameHeight();
	const float w_extra = (flags & ImGuiColorEditFlags_NoSmallPreview) ? 0.0f : (square_sz + style.ItemInnerSpacing.x);
	const float w_items_all = CalcItemWidth() - w_extra;
	const char* label_display_end = FindRenderedTextEnd(label);

	BeginGroup();
	PushID(label);

	// If we're not showing any slider there's no point in doing any HSV conversions
	const ImGuiColorEditFlags flags_untouched = flags;
	if (flags & ImGuiColorEditFlags_NoInputs)
		flags = (flags & (~ImGuiColorEditFlags__InputMask)) | ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_NoOptions;

	// Context menu: display and modify options (before defaults are applied)
	if (!(flags & ImGuiColorEditFlags_NoOptions))
		ColorEditOptionsPopup(col, flags);

	// Read stored options
	if (!(flags & ImGuiColorEditFlags__InputMask))
		flags |= (g.ColorEditOptions & ImGuiColorEditFlags__InputMask);
	if (!(flags & ImGuiColorEditFlags__DataTypeMask))
		flags |= (g.ColorEditOptions & ImGuiColorEditFlags__DataTypeMask);
	if (!(flags & ImGuiColorEditFlags__PickerMask))
		flags |= (g.ColorEditOptions & ImGuiColorEditFlags__PickerMask);
	flags |= (g.ColorEditOptions & ~(ImGuiColorEditFlags__InputMask | ImGuiColorEditFlags__DataTypeMask | ImGuiColorEditFlags__PickerMask));

	const bool alpha = (flags & ImGuiColorEditFlags_NoAlpha) == 0;
	const bool hdr = (flags & ImGuiColorEditFlags_HDR) != 0;
	const int components = alpha ? 4 : 3;

	// Convert to the formats we need
	float f[4] = { col[0], col[1], col[2], alpha ? col[3] : 1.0f };
	if (flags & ImGuiColorEditFlags_HSV)
		ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);
	int i[4] = { IM_F32_TO_INT8_UNBOUND(f[0]), IM_F32_TO_INT8_UNBOUND(f[1]), IM_F32_TO_INT8_UNBOUND(f[2]), IM_F32_TO_INT8_UNBOUND(f[3]) };

	bool value_changed = false;
	bool value_changed_as_float = false;

	if ((flags & (ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_HSV)) != 0 && (flags & ImGuiColorEditFlags_NoInputs) == 0)
	{
		// RGB/HSV 0..255 Sliders
		const float w_item_one = ImMax(1.0f, (float)(int)((w_items_all - (style.ItemInnerSpacing.x) * (components - 1)) / (float)components));
		const float w_item_last = ImMax(1.0f, (float)(int)(w_items_all - (w_item_one + style.ItemInnerSpacing.x) * (components - 1)));

		const bool hide_prefix = (w_item_one <= CalcTextSize((flags & ImGuiColorEditFlags_Float) ? "M:0.000" : "M:000").x);
		const char* ids[4] = { "##X", "##Y", "##Z", "##W" };
		const char* fmt_table_int[3][4] =
		{
			{   "%3d",   "%3d",   "%3d",   "%3d" }, // Short display
			{ "R:%3d", "G:%3d", "B:%3d", "A:%3d" }, // Long display for RGBA
			{ "H:%3d", "S:%3d", "V:%3d", "A:%3d" }  // Long display for HSVA
		};
		const char* fmt_table_float[3][4] =
		{
			{   "%0.3f",   "%0.3f",   "%0.3f",   "%0.3f" }, // Short display
			{ "R:%0.3f", "G:%0.3f", "B:%0.3f", "A:%0.3f" }, // Long display for RGBA
			{ "H:%0.3f", "S:%0.3f", "V:%0.3f", "A:%0.3f" }  // Long display for HSVA
		};
		const int fmt_idx = hide_prefix ? 0 : (flags & ImGuiColorEditFlags_HSV) ? 2 : 1;

		PushItemWidth(w_item_one);
		for (int n = 0; n < components; n++)
		{
			if (n > 0)
				SameLine(0, style.ItemInnerSpacing.x);
			if (n + 1 == components)
				PushItemWidth(w_item_last);
			if (flags & ImGuiColorEditFlags_Float)
			{
				value_changed |= DragFloat(ids[n], &f[n], 1.0f / 255.0f, 0.0f, hdr ? 0.0f : 1.0f, fmt_table_float[fmt_idx][n]);
				value_changed_as_float |= value_changed;
			}
			else
			{
				value_changed |= DragInt(ids[n], &i[n], 1.0f, 0, hdr ? 0 : 255, fmt_table_int[fmt_idx][n]);
			}
			if (!(flags & ImGuiColorEditFlags_NoOptions))
				OpenPopupOnItemClick("context");
		}
		PopItemWidth();
		PopItemWidth();
	}
	else if ((flags & ImGuiColorEditFlags_HEX) != 0 && (flags & ImGuiColorEditFlags_NoInputs) == 0)
	{
		// RGB Hexadecimal Input
		char buf[64];
		if (alpha)
			ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255), ImClamp(i[3], 0, 255));
		else
			ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255));
		PushItemWidth(w_items_all);
		if (InputText("##Text", buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase))
		{
			value_changed = true;
			char* p = buf;
			while (*p == '#' || ImCharIsBlankA(*p))
				p++;
			i[0] = i[1] = i[2] = i[3] = 0;
			if (alpha)
				sscanf(p, "%02X%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2], (unsigned int*)&i[3]); // Treat at unsigned (%X is unsigned)
			else
				sscanf(p, "%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2]);
		}
		if (!(flags & ImGuiColorEditFlags_NoOptions))
			OpenPopupOnItemClick("context");
		PopItemWidth();
	}

	ImGuiWindow* picker_active_window = NULL;
	if (!(flags & ImGuiColorEditFlags_NoSmallPreview))
	{
		if (!(flags & ImGuiColorEditFlags_NoInputs))
			SameLine(0, style.ItemInnerSpacing.x);

		const ImVec4 col_v4(col[0], col[1], col[2], alpha ? col[3] : 1.0f);
		if (ColorButton("##ColorButton", col_v4, flags))
		{
			if (!(flags & ImGuiColorEditFlags_NoPicker))
			{
				// Store current color and open a picker
				g.ColorPickerRef = col_v4;
				OpenPopup("picker");
				SetNextWindowPos(window->DC.LastItemRect.GetBL() + ImVec2(-1, style.ItemSpacing.y));
			}
		}
		if (!(flags & ImGuiColorEditFlags_NoOptions))
			OpenPopupOnItemClick("context");

		if (BeginPopup("picker"))
		{
			picker_active_window = g.CurrentWindow;
			if (label != label_display_end)
			{
				TextUnformatted(label, label_display_end);
				Spacing();
			}
			ImGuiColorEditFlags picker_flags_to_forward = ImGuiColorEditFlags__DataTypeMask | ImGuiColorEditFlags__PickerMask | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaBar;
			ImGuiColorEditFlags picker_flags = (flags_untouched & picker_flags_to_forward) | ImGuiColorEditFlags__InputMask | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;
			PushItemWidth(square_sz * 12.0f); // Use 256 + bar sizes?
			value_changed |= ColorPicker4("##picker", col, picker_flags, &g.ColorPickerRef.x);
			PopItemWidth();
			EndPopup();
		}
	}

	if (label != label_display_end && !(flags & ImGuiColorEditFlags_NoLabel))
	{
		SameLine(0, style.ItemInnerSpacing.x);
		TextUnformatted(label, label_display_end);
	}

	// Convert back
	if (picker_active_window == NULL)
	{
		if (!value_changed_as_float)
			for (int n = 0; n < 4; n++)
				f[n] = i[n] / 255.0f;
		if (flags & ImGuiColorEditFlags_HSV)
			ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
		if (value_changed)
		{
			col[0] = f[0];
			col[1] = f[1];
			col[2] = f[2];
			if (alpha)
				col[3] = f[3];
		}
	}

	PopID();
	EndGroup();

	// Drag and Drop Target
	// NB: The flag test is merely an optional micro-optimization, BeginDragDropTarget() does the same test.
	if ((window->DC.LastItemStatusFlags & ImGuiItemStatusFlags_HoveredRect) && !(flags & ImGuiColorEditFlags_NoDragDrop) && BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
		{
			memcpy((float*)col, payload->Data, sizeof(float) * 3); // Preserve alpha if any //-V512
			value_changed = true;
		}
		if (const ImGuiPayload* payload = AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
		{
			memcpy((float*)col, payload->Data, sizeof(float) * components);
			value_changed = true;
		}
		EndDragDropTarget();
	}

	// When picker is being actively used, use its active id so IsItemActive() will function on ColorEdit4().
	if (picker_active_window && g.ActiveId != 0 && g.ActiveIdWindow == picker_active_window)
		window->DC.LastItemId = g.ActiveId;

	if (value_changed)
		MarkItemEdited(window->DC.LastItemId);

	return value_changed;
}
using namespace std;

auto get_lua_dir = [&]()
{
	auto dir_path = file_tools::get_hack_directory_path();
	
	return dir_path.string();
};
void read_file(std::string& out, const std::string& path)
{
	std::ifstream file;
	file.open(path);
	std::string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	out = file_content;
}	
void main_window::draw_main_window() {
	draw_entity_list();
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
			if (tab("Credits", ICON_FA_WHEELCHAIR, selectedtab == 4))
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
				if (subtab("Colors", selectedsubtab == 1))
					selectedsubtab = 1;
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
					checkbox("Enable", &globals::aimbotenable); // aimbot_autofire norecoil nospread aimbot_fov_draw aimbot_silent aimbot_draw_target
					checkbox("AutoFire", &settings::get_bool("aimbot_autofire"));
					checkbox("NoRecoil", &settings::get_bool("norecoil"));
					checkbox("NoSpread", &settings::get_bool("nospread"));
					checkbox("Silent", &settings::get_bool("aimbot_silent"));
					int aa;
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
					Hotkey("Aimbot Key", &globals::aimbotkey);
				}
				else if (selectedsubtab == 0 && selectedcategory == 1)
				{
					checkbox("AimBot Fov Draw", &settings::get_bool("aimbot_fov_draw"));
					ImGui::SameLine();
					ImGui::SetCursorPosY(100);
					checkbox("Aimbot Draw Target", &settings::get_bool("aimbot_draw_target"));
					ImGui::SameLine();
					auto aa = ImGui::GetCursorPosY();
				}
			}
			else if(selectedtab==2)
			{
				if (selectedsubtab == 0) 
				{
					static Wittchen::WitthcenEspStyleEditor g_style_editor;
					static const char* text[]{ "border", "corner" };
					slider_float("Esp Draw Distance", &settings::get_float("esp_dist"), 0, 20000, NULL, NULL);
					checkbox("Esp Enable", &settings::get_bool("esp_enable"));
					checkbox("Players Info", &settings::get_bool("esp_info"));
					combo("Box Type", &settings::get_int("esp_type"), text, IM_ARRAYSIZE(text));
					if (button("Entity List", ImVec2(303, 25)))
						drawentlist = !drawentlist;
				}
				if (selectedsubtab == 1)
				{
					ColorEdit44("FOV", globals::colorfov, ImGuiColorEditFlags_NoInputs);
					ColorEdit44("Target", globals::colortarger, ImGuiColorEditFlags_NoInputs);
					ColorEdit44("Friend Esp", globals::colorfriend, ImGuiColorEditFlags_NoInputs);
					ColorEdit44("Esp", globals::colorespplayer, ImGuiColorEditFlags_NoInputs);
					ColorEdit44("Entity Esp", globals::colorespentity, ImGuiColorEditFlags_NoInputs);
				}
				//slider_int("ESP Draw distance", &settings::get_int("esp_dist"), 0, 50000,NULL,NULL);
			}
			else if (selectedtab == 3)
			{
				if (selectedsubtab == 0) {
					checkbox("Bunnyhop", &settings::get_bool("bhop"));
					checkbox("AutoStrafe", &settings::get_bool("autostrafe"));
					checkbox("FixMovement", &settings::get_bool("fixmovement"));
					checkbox("ThirdPerson", &settings::get_bool("third_person"));
					slider_int("Custom Viewmodel Fov", &settings::get_int("custom_viewmodel_fov"), 0, 180, NULL, NULL);
					slider_int("Custom Fov", &settings::get_int("custom_fov"), 0, 180, NULL, NULL);
					slider_int("Custom Aspect Ratio", &settings::get_int("custom_aspect_ratio"), 0, 180, NULL, NULL);
					slider_int("ThirdPerson Distance", &settings::get_int("third_person_distance"), 0, 180, NULL, NULL);
					Hotkey("ThirdPerson Key", &globals::thirdpersonkey);
				}
				else if (selectedsubtab == 2)
				{
					static std::vector<std::string> configs;
					static std::string current_config;
					static auto load_configs = []() {
						std::vector<std::string> items = {};

						std::string path= get_lua_dir(); 
						if (!filesystem::is_directory(path))
							filesystem::create_directories(path);

						for (auto& p : filesystem::directory_iterator(path))
							items.push_back(p.path().string());

						return items;
					};

					static auto is_configs_loaded = false;
					if (!is_configs_loaded) {
						is_configs_loaded = true;
						configs = load_configs();
					}
					ImGui::SetCursorPosX(200);
					ImGui::ListBoxHeader("##luas", {338,230});
					{
						auto clean_item_name = [](const char* name) -> const char* {
							if (name[0] == 'C')
								name++;

							auto start = strstr(name, "Roaming");
							if (start != nullptr)
								name = start + 8;

							return name;
						};
						for (auto& config : configs) {
							auto isexeccfg = strstr(config.c_str(), ".lua") != nullptr;
							if (isexeccfg) {
								if (ImGui::Selectable(clean_item_name(config.c_str()), config == current_config)) {
									current_config = config;
									globals::lua = current_config;
								}
							}
						}
					}
					ImGui::ListBoxFooter();
					static bool isexeccfg;
					if (button("Run", ImVec2(303, 25)))
					{
						std::string code;
						read_file(code,globals::lua);
						lua_futures::add_code_to_run(code);
					}
					if (button("Refresh", ImVec2(303, 25)))
						is_configs_loaded = false;
					if (button("Open Folder", ImVec2(303, 25)))
					{
						ShellExecute(NULL, NULL, get_lua_dir().c_str(), NULL, NULL, SW_SHOWNORMAL);
					}
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
