#pragma once

#include <array>

#include <d3d9.h>
#include <functional>
#include <imgui/im_tools.h>

#include "color.h"

namespace render_system {
	namespace fonts {
		inline std::array<ImFont*, 3> nunito_font;

		inline std::array<ImFont*, 3> menu_font;
		
		inline ImFont* arial_font;
	}

	
	void init();
	IDirect3DDevice9* get_device();

	void on_end_scene(LPDIRECT3DDEVICE9 device, uintptr_t return_address);
}

namespace imgui_render {
	void corner_box(ImDrawList* list, const ImVec2& min, const ImVec2& max, c_color color);
}

namespace directx_render
{
	enum e_font_flags
	{
		font_none = 0,
		font_centered_x = (1 << 0),
		font_centered_y = (1 << 1),
		font_centered = font_centered_x | font_centered_y,
		font_drop_shadow = (1 << 2),
		font_outline = (1 << 3)
	};

	void init_directx_render();

	void render_surface(std::function<void()> draw_surface);

	void filled_rect(const ImVec2& min, const ImVec2& max, c_color color, float round = 0.f);
	void bordered_rect(const ImVec2& min, const ImVec2& max, c_color color, float round = 0.f);
	void line(const ImVec2& pos1, const ImVec2& pos2, c_color color, float t = 1.f);
	void text(ImFont* font, const std::string& text, const ImVec2& pos, float size, c_color color, int flags);

	void outlined_circle(const ImVec2& pos, int radius, c_color color, int segments = 120, int t = 1);
	void filled_circle(const ImVec2& pos, int radius, c_color color, int segments = 120, int t = 1);

	void corner_box(const ImVec2& min, const ImVec2& max, c_color color);

}