#pragma once

#include <string>
#include <imgui/im_tools.h>

#include "../../game_sdk/entities/c_base_entity.h"
#include "../../render_system/render_system.h"

#include <unordered_map>

namespace esp {
	struct esp_text_t {
		std::string text;
		int flags = directx_render::e_font_flags::font_outline;
		float size;
		c_color color;
		bool is_auto_color;
		int relative_position;
	};

	enum class e_esp_text_position : uint32_t {top = 0, right, down, left};
	enum class box_type : int {filled, border, corner};
	
	struct esp_text_storage_t : std::array<std::pair<ImVec2, std::vector<esp_text_t>>, 4>{
		std::unordered_map<uint64_t, esp_text_t> strings;
		std::array<ImVec2, 4> last_positions;
	};
	
	class c_esp_box {
	public:
		esp_text_storage_t text_storage;
		ImVec2 min, max;
		float rounding;
		int type = (int)box_type::filled;
		c_color color;
		c_color border_color;
		
		static void get_absolute_position(const ImVec2& r);
		ImVec2 get_screen_position(const ImVec2& pos) const;
		
		static bool calc_box(c_base_entity* ent, c_esp_box& box);
		static ImVec2 calc_text_position(const c_esp_box& box, esp_text_t& text, std::array<ImVec2, 4>& last_positions);
		static uint64_t generate_id();
	};

	
	void draw_esp();
}
