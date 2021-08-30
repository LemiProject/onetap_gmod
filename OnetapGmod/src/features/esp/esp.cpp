#include "esp.h"

#include "../../game_sdk/entities/c_base_player.h"
#include <algorithm>

#include <lemi_utils.h>

#include "../../utils/game_utils.h"

#include "../../render_system/render_system.h"
#include "../../globals.h"
#include "../menu/widgets/widgets.h"
#include "../../settings/settings.h"
#include "../../game_sdk/entities/c_base_weapon.h"

/// Formatter usage
/// Player:
///		%name - player name
///		%health - player health in procesnt`s
///		%armour - player armour in procesnt`s //not impl
///		%weapon - player weapon
///		%group - player group
///		%team - player team
///		%id - steam id
///		%rpname - rpname of player
///		%ammo - current weapon ammo
///
///	Entity:
///		%name - entity name
///		%health - player health in procesnt`s

std::string format_text_for_player(const std::string& str, c_base_player* ply) {
	auto s = str;
	if (settings::get_bool("esp_info"))
	{
		auto weapon = ply->get_active_weapon();
		if (weapon)
		{
			auto weapon_name = weapon->get_print_name();
			if (!weapon_name.c_str())
				weapon_name = "None";
			s = replace_all(s, "%activeweapon", weapon_name);
		}
		s = replace_all(s, "%name", ply->get_name());
		s = replace_all(s, "%health", std::to_string(ply->get_health()));
		s = replace_all(s, "%team_name", ply->get_team_name());
		s = replace_all(s, "%user_group", ply->get_user_group());

		return s;
	}
}

void format_esp_map_for_players(std::unordered_map<uint64_t, esp::esp_text_t>& t, c_base_player* ply) {
	for (auto& i : t) {
		i.second.color = i.second.is_auto_color ? Wittchen::WitthcenEspStyleEditor::GetAutoColor(i.second.text, ply) : i.second.color;
		i.second.text = format_text_for_player(i.second.text, ply);
	}
}

__forceinline float calc_font_size(const esp::c_esp_box& box) {
	return 15.f;
}

void esp::c_esp_box::get_absolute_position(const ImVec2& r) {
	
}

ImVec2 esp::c_esp_box::get_screen_position(const ImVec2& pos) const {
	return {min.x + pos.x, min.y + pos.y};
}

bool esp::c_esp_box::calc_box(c_base_entity* ent, c_esp_box& box) {
	using namespace game_utils;
	c_vector flb, brt, blb, frt, frb, brb, blt, flt;

	const auto& origin = ent->get_render_origin();
	const auto min = ent->get_collidable_ptr()->mins() + origin;
	const auto max = ent->get_collidable_ptr()->maxs() + origin;

	c_vector points[] = {
		c_vector(min.x, min.y, min.z),
		c_vector(min.x, max.y, min.z),
		c_vector(max.x, max.y, min.z),
		c_vector(max.x, min.y, min.z),
		c_vector(max.x, max.y, max.z),
		c_vector(min.x, max.y, max.z),
		c_vector(min.x, min.y, max.z),
		c_vector(max.x, min.y, max.z)
	};

	if (!world_to_screen(points[3], flb) || !world_to_screen(points[5], brt)
		|| !world_to_screen(points[0], blb) || !world_to_screen(points[4], frt)
		|| !world_to_screen(points[2], frb) || !world_to_screen(points[1], brb)
		|| !world_to_screen(points[6], blt) || !world_to_screen(points[7], flt))
		return false;

	c_vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	auto left = flb.x;
	auto top = flb.y;
	auto right = flb.x;
	auto bottom = flb.y;

	if (left < 0 || top < 0 || right < 0 || bottom < 0)
		return false;

	for (int i = 1; i < 8; i++) {
		if (left > arr[i].x)
			left = arr[i].x;
		if (bottom < arr[i].y)
			bottom = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (top > arr[i].y)
			top = arr[i].y;
	}

	box.min = { left, top };
	box.max = {right, bottom};

	return true;
}

ImVec2 esp::c_esp_box::calc_text_position(const c_esp_box& box, esp_text_t& text, std::array<ImVec2, 4>& last_positions) {
	const ImVec2 box_size = { box.max.x - box.min.x, box.max.y - box.min.y };
	
	if (text.relative_position == (int)esp::e_esp_text_position::top) {
		auto& last_position = last_positions[(int)esp::e_esp_text_position::top];
		auto font_size = (text.size == -1 || text.size == 0 ) ? calc_font_size(box) : text.size;
		auto text_size = render_system::fonts::nunito_font[2]->CalcTextSizeA(font_size, FLT_MAX, 0.f, text.text.c_str());
		ImVec2 base_position = { box_size.x / 2.f, -(text_size.y / 2.f) };

		ImVec2 position = { base_position.x, last_position.y != -1.f ? last_position.y - text_size.y : base_position.y };

		//only centered text
		text.flags = !(text.flags & directx_render::font_centered) ? text.flags |= directx_render::font_centered : text.flags;

		last_position = position;
		return position;
	}

	if (text.relative_position == (int)esp::e_esp_text_position::right) {
		auto& last_position = last_positions[(int)esp::e_esp_text_position::right];
		auto font_size = (text.size == -1 || text.size == 0) ? calc_font_size(box) : text.size;
		auto text_size = render_system::fonts::nunito_font[2]->CalcTextSizeA(font_size, FLT_MAX, 0.f, text.text.c_str());
		ImVec2 base_position = { box_size.x + text_size.y / 2.f, 0 };
		ImVec2 position = { base_position.x, last_position.y != -1.f ? last_position.y + text_size.y : base_position.y };

		last_position = position;
		return position;
	}

	if (text.relative_position == (int)esp::e_esp_text_position::left) {
		auto& last_position = last_positions[(int)esp::e_esp_text_position::left];
		auto font_size = (text.size == -1 || text.size == 0) ? calc_font_size(box) : text.size;
		auto text_size = render_system::fonts::nunito_font[2]->CalcTextSizeA(font_size, FLT_MAX, 0.f, text.text.c_str());
		ImVec2 base_position = { -text_size.x - (text_size.y / 2.f), 0 };
		ImVec2 position = { base_position.x, last_position.y != -1.f ? last_position.y + text_size.y : base_position.y };

		last_position = position;
		return position;
	}

	if (text.relative_position == (int)esp::e_esp_text_position::down) {
		auto& last_position = last_positions[(int)esp::e_esp_text_position::down];
		auto font_size = (text.size == -1 || text.size == 0) ? calc_font_size(box) : text.size;
		auto text_size = render_system::fonts::nunito_font[2]->CalcTextSizeA(font_size, FLT_MAX, 0.f, text.text.c_str());
		ImVec2 base_position = { box_size.x / 2.f, box_size.y + (text_size.y / 2.f) };

		ImVec2 position = { base_position.x, last_position.y != -1.f ? last_position.y + text_size.y : base_position.y };

		//only centered text
		text.flags = !(text.flags & directx_render::font_centered) ? text.flags |= directx_render::font_centered : text.flags;

		last_position = position;
		return position;
	}

	return { 0, 0 };
}

uint64_t esp::c_esp_box::generate_id() {
	static uint64_t last;
	last++;
	return last;
}

void render_strings_for_players(esp::c_esp_box& box, c_base_player* player) {
	//format strings first
	format_esp_map_for_players(box.text_storage.strings, player);

	//constants
	box.text_storage.last_positions.fill({ -1, -1 });
	
	//render strings
	{
		for (auto& i : box.text_storage.strings) {
			auto position = esp::c_esp_box::calc_text_position(box, i.second, box.text_storage.last_positions);
			const auto font_size = (i.second.size == -1 || i.second.size == 0) ? calc_font_size(box) : i.second.size;
			directx_render::text(render_system::fonts::nunito_font[2], i.second.text, box.get_screen_position(position), font_size, i.second.color, i.second.flags);
		}
	}
}
bool get_entity_box(c_base_entity* ent, math::box_t& box_in)
{
	using namespace game_utils;

	c_vector flb, brt, blb, frt, frb, brb, blt, flt;

	const auto& origin = ent->get_render_origin();
	const auto min = ent->get_collidable_ptr()->mins() + origin;
	const auto max = ent->get_collidable_ptr()->maxs() + origin;

	c_vector points[] = {
		c_vector(min.x, min.y, min.z),
		c_vector(min.x, max.y, min.z),
		c_vector(max.x, max.y, min.z),
		c_vector(max.x, min.y, min.z),
		c_vector(max.x, max.y, max.z),
		c_vector(min.x, max.y, max.z),
		c_vector(min.x, min.y, max.z),
		c_vector(max.x, min.y, max.z)
	};

	if (!world_to_screen(points[3], flb) || !world_to_screen(points[5], brt)
		|| !world_to_screen(points[0], blb) || !world_to_screen(points[4], frt)
		|| !world_to_screen(points[2], frb) || !world_to_screen(points[1], brb)
		|| !world_to_screen(points[6], blt) || !world_to_screen(points[7], flt))
		return false;

	c_vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	auto left = flb.x;
	auto top = flb.y;
	auto right = flb.x;
	auto bottom = flb.y;

	if (left < 0 || top < 0 || right < 0 || bottom < 0)
		return false;

	for (int i = 1; i < 8; i++) {
		if (left > arr[i].x)
			left = arr[i].x;
		if (bottom < arr[i].y)
			bottom = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (top > arr[i].y)
			top = arr[i].y;
	}

	box_in.x = left;
	box_in.y = top;
	box_in.w = right - left;
	box_in.h = bottom - top;

	return true;
}
inline float calc_text_size(c_base_entity* ent, math::box_t box)
{
	float size = 16.f;
	if (box.h < 55)
	{
		size -= size * (box.h / 100);
		if (size < 13.f)
			size = 13.f;
	}
	return size;
}
ImVec2 last_text_pos = { -1.f, -1.f };
std::string last_text;
void draw_health(c_base_entity* ent, math::box_t box,c_color color)
{
	if (ent->is_alive())
	{
		const auto health = std::clamp(ent->get_health(), 0, 100);

		if (!ent->is_player())
		{
			const auto pos = ImVec2(box.x - 6.f, box.y);

			const math::box_t void_box{ pos.x, pos.y, 3, box.h };
			auto health_box{ void_box };

			health_box.h = health * void_box.h / 100;
			health_box.y = void_box.y + void_box.h - health_box.h;

			//const auto health_color = c_color(settings::colors::colors_map["esp_health_color_hp"]);
			//const auto void_color = c_color(settings::colors::colors_map["esp_health_color_void"]);

//			directx_render::filled_rect(void_box, void_color);
	//		directx_render::filled_rect(health_box, health_color);
		}

		if (!ent->is_player())
		{
			{

				auto text = std::to_string(health);
				auto font_size = calc_text_size(ent, box);
				auto text_size = render_system::fonts::arial_font->CalcTextSizeA(font_size, FLT_MAX, 0, text.c_str());
				auto text_pos = last_text_pos;

				text_pos.x += text_size.y / 2.f;

				float g = 255 * (health / 100.f);
				c_color text_color = { 255 - g, g, 0 };

				directx_render::text(render_system::fonts::arial_font, text, text_pos, font_size, color, directx_render::font_outline);

				last_text_pos = text_pos;
				last_text = text;
			}
		}
	}
}

inline void draw_name(c_base_entity* ent, math::box_t& box, c_color color)
{
	const auto text = ent->is_player() ? static_cast<c_base_player*>(ent)->get_name() : ent->get_print_name();

	auto font_size = calc_text_size(ent, box);
	auto ts = render_system::fonts::arial_font->CalcTextSizeA(font_size, FLT_MAX, 0.f, text.c_str());
	const math::vec2_t text_size = { ts.x, ts.y };

	//const auto position = math::vec2_t{ box.x + box.w * 0.5f/* - (text_size.x / 2.f)*/, box.y + box.h + text_size.y / 2};
	const auto position = math::vec2_t{ box.x + box.w * 0.5f, box.y - text_size.y / 2 };

	//const auto color = ent->is_player() ? static_cast<c_base_player*>(ent)->get_team_color() : c_color(1.0f,0,0);

	directx_render::text(render_system::fonts::arial_font, text, position.get_im_vec2(), font_size, color, directx_render::font_centered | directx_render::font_outline);

	/*if (ent->is_player())
	{
		auto sid = static_cast<c_base_player*>(ent)->get_steam_id();
		if (!sid.empty())
		{
			if (std::find(settings::other::friends.begin(), settings::other::friends.end(), sid) != settings::other::friends.end())
			{
				constexpr auto str = "F";

				auto name_size = ts;
				ts = render_system::fonts::in_game_font->CalcTextSizeA(font_size, FLT_MAX, 0.f, str);
				auto pos = ImVec2(box.x + box.w + ts.y / 2.f, box.y);

				if (last_text_pos.x >= 0 && last_text_pos.y >= 0)
					pos.y = last_text_pos.y + render_system::fonts::in_game_font->CalcTextSizeA(font_size, FLT_MAX, 0.f, last_text.c_str()).y;

				directx_render::text(render_system::fonts::in_game_font, str, pos, font_size, colors::green_color, directx_render::font_outline);

				last_text_pos = pos;
				last_text = str;
			}
		}
	}*/

}
inline void draw_box(c_base_entity* ent, math::box_t& box)
{
	
		directx_render::corner_box1(box, c_color(1.0f,1.0f,1.0f));
	
}
void esp::draw_esp() {
	if (!settings::get_bool("esp_enable"))
		return;
	//at this time only players
	if (!interfaces::engine->is_in_game())
		return;

	
	for (auto i : game_utils::get_valid_players(true)) {
		auto p = get_player_by_index(i);
		if (get_local_player()->get_eye_pos().distance_to(p->get_eye_pos()) > settings::get_float("esp_dist"))
			continue;
		c_esp_box box;
		if (!c_esp_box::calc_box(p, box))
			continue;
		Wittchen::ApplyStyleToBox(box);

		if (p->is_player())
		{
			render_strings_for_players(box, p);
			switch ((box_type)box.type) {
			case box_type::filled:
				directx_render::bordered_rect(box.min, box.max, box.color, box.rounding);
				break;
			case box_type::border:
				directx_render::bordered_rect({ box.min.x - 1.f, box.min.y - 1.f }, { box.max.x + 1.f, box.max.y + 1.f }, box.border_color, box.rounding);
				directx_render::bordered_rect({ box.min.x + 1.f, box.min.y + 1.f }, { box.max.x - 1.f, box.max.y - 1.f }, box.border_color, box.rounding);
				directx_render::bordered_rect(box.min, box.max, box.color, box.rounding);
				break;
			case box_type::corner:
				directx_render::corner_box(box.min, box.max, box.color);
			}
		}
		else if (!p->is_player())
		{
			const auto is_draw = settings::entitys_to_draw.exist(p->get_classname());
			//const auto has_owner = interfaces::entity_list->get_entity_by_handle(ent->get_owner_entity_handle()) ? true : false;

			if (!is_draw)
				continue;

			math::box_t box1{};


			if (!get_entity_box(p, box1))
				continue;
			draw_box(p, box1);

			draw_health(p, box1,box.color);

			draw_name(p, box1, box.color);
		}
	}
}
