#include "themes.h"

#include <imgui/im_tools.h>
#include <fontawesomium.h>
#include "sffont.hpp"
#include "hashes.h"
#include "faprolight.hpp"


using namespace ImGui;
static const ImWchar ranges[] =
{
	0x0020, 0x00FF, // Basic Latin + Latin Supplement
	0x2000, 0x206F, // General Punctuation
	0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
	0x31F0, 0x31FF, // Katakana Phonetic Extensions
	0xFF00, 0xFFEF, // Half-width characters
	0x4e00, 0x9FAF, // CJK Ideograms
	0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
	0x2DE0, 0x2DFF, // Cyrillic Extended-A
	0xA640, 0xA69F, // Cyrillic Extended-B
	0,
};
void menu_themes::standard_theme() {
	auto& io = GetIO;
	auto& style = GetStyle();

	style.WindowPadding = ImVec2(0.000000f,0.000000f);
 		style.FramePadding = ImVec2(4.000000f,3.000000f);
 		style.ItemSpacing = ImVec2(8.000000f,4.000000f);
 		style.ItemInnerSpacing = ImVec2(4.000000f,4.000000f);
 		style.IndentSpacing = 21.000000f;
 		style.ScrollbarSize = 1.000000f;
 		style.GrabMinSize = 1.000000f;
 		style.WindowBorderSize = 0.000000f;
 		style.ChildBorderSize = 1.000000f;
 		style.PopupBorderSize = 1.000000f;
 		style.FrameBorderSize = 1.000000f;
 		style.WindowRounding = 0.000000f;
 		style.ChildRounding = 0.000000f;
 		style.FrameRounding = 0.000000f;
 		style.PopupRounding = 0.000000f;
 		style.ScrollbarRounding = 0.000000f;
 		style.GrabRounding = 0.000000f;
 		style.WindowTitleAlign = ImVec2(0.000000f,0.500000f);
 		style.ButtonTextAlign = ImVec2(0.500000f,0.500000f);
 		style.SelectableTextAlign = ImVec2(0.500000f,0.500000f);

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Tab] = ImColor(155, 155, 159, 255);
	colors[ImGuiCol_TabHovered] = ImColor(145, 145, 149, 255);
	colors[ImGuiCol_TabActive] = ImColor(155, 155, 159, 255);
	colors[ImGuiCol_Text] = ImVec4(0.900000f, 0.900000f, 0.900000f, 1.000000f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.600000f, 0.600000f, 0.600000f, 1.000000f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.000000f, 0.000000f, 0.000000f, 0.000000f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.000000f, 0.000000f, 0.000000f, 0.000000f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.110000f, 0.110000f, 0.140000f, 0.920000f);
	colors[ImGuiCol_Border] = ImVec4(0.500000f, 0.500000f, 0.500000f, 0.500000f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.000000f, 0.000000f, 0.000000f, 0.000000f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.430000f, 0.430000f, 0.430000f, 0.390000f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.470000f, 0.470000f, 0.690000f, 0.400000f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.420000f, 0.410000f, 0.640000f, 0.690000f);
	colors[ImGuiCol_TitleBg] = ImColor(249, 165, 22, 255);
	colors[ImGuiCol_TitleBgActive] = ImColor(249, 165, 22, 255);
	colors[ImGuiCol_TitleBgCollapsed] = ImColor(249, 165, 22, 255);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.400000f, 0.400000f, 0.550000f, 0.800000f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.200000f, 0.250000f, 0.300000f, 0.600000f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.400000f, 0.400000f, 0.800000f, 0.300000f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.400000f, 0.400000f, 0.800000f, 0.400000f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.410000f, 0.390000f, 0.800000f, 0.600000f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.900000f, 0.900000f, 0.900000f, 0.500000f);
	colors[ImGuiCol_SliderGrab] = ImVec4(1.000000f, 1.000000f, 1.000000f, 0.300000f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.410000f, 0.390000f, 0.800000f, 0.600000f);
	colors[ImGuiCol_Button] = ImVec4(0.350000f, 0.400000f, 0.610000f, 0.620000f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.400000f, 0.480000f, 0.710000f, 0.790000f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.460000f, 0.540000f, 0.800000f, 1.000000f);
	colors[ImGuiCol_Header] = ImVec4(0.400000f, 0.400000f, 0.900000f, 0.450000f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.450000f, 0.450000f, 0.900000f, 0.800000f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.530000f, 0.530000f, 0.870000f, 0.800000f);
	colors[ImGuiCol_Separator] = ImVec4(0.500000f, 0.500000f, 0.500000f, 0.600000f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.600000f, 0.600000f, 0.700000f, 1.000000f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.700000f, 0.700000f, 0.900000f, 1.000000f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(1.000000f, 1.000000f, 1.000000f, 0.160000f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.780000f, 0.820000f, 1.000000f, 0.600000f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.780000f, 0.820000f, 1.000000f, 0.900000f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.000000f, 0.000000f, 1.000000f, 0.350000f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.000000f, 1.000000f, 0.000000f, 0.900000f);

	ImFontConfig font_cfg{};
	font_cfg.FontDataOwnedByAtlas = false;
	ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(sfpro_compressed_data, sfpro_compressed_size, 14, &font_cfg, ranges);

	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
	ImGui::GetIO().Fonts->AddFontFromMemoryTTF(&faprolight, sizeof faprolight, 14, &icons_config, icon_ranges);
}
