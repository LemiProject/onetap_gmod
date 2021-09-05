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
#include <stdio.h>

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

std::string format_text_for_entity(const std::string& str, c_base_entity* ent) {
	auto s = str;
	if (ent->is_player()) {
		auto ply = (c_base_player*)ent;
		char dist[256];
		if (settings::get_bool("esp_player_dist")) {
			sprintf_s(dist, "%.1fm", (get_local_player()->get_origin() - ent->get_origin()).length() * 0.0254f);
			s = replace_all(s, "%distance", dist);
		}
			auto weapon = ply->get_active_weapon();
			if (weapon)
			{
				auto weapon_name = weapon->get_print_name();
				if (!weapon_name.c_str())
					weapon_name = "None";
				if (settings::get_bool("esp_player_weapon"))
					s = replace_all(s, "%activeweapon", weapon_name);
			}
			if (settings::get_bool("esp_player_name"))
				s = replace_all(s, "%name", ply->get_name());
			if (settings::get_bool("esp_player_rpname"))
				s = replace_all(s, "%rpname", ply->GetName(ply->get_index()));
			if (settings::get_bool("esp_player_hp"))
				s = replace_all(s, "%health", std::to_string(ply->get_health()));
			if (settings::get_bool("esp_player_team"))
				s = replace_all(s, "%team_name", ply->get_team_name());
			if (settings::get_bool("esp_player_group"))
				s = replace_all(s, "%user_group", ply->get_user_group());
			s = replace_all(s, "%distance", "");

			return s;

		
	} 
	else if(!ent->is_player())
	{
		
			char dist[256];
			if (settings::get_bool("esp_entitie_dist")) {
				sprintf_s(dist, "%.1fm", (get_local_player()->get_origin() - ent->get_origin()).length() * 0.0254f);
				s = replace_all(s, "%distance", dist);
			}
			if (settings::get_bool("esp_entitie_name"))
				s = replace_all(s, "%name", ent->get_classname());
			if (ent->get_health() > 0&& settings::get_bool("esp_entitie_hp"))
				s = replace_all(s, "%health", std::to_string(ent->get_health()));
			s = replace_all(s, "%team_name", "");
			s = replace_all(s, "%user_group", "");
			s = replace_all(s, "%activeweapon", "");

			return s;
		
	}
	return "";
}

void format_esp_map_for_players(std::unordered_map<uint64_t, esp::esp_text_t>& t, c_base_player* ply) {
	for (auto& i : t) {
		i.second.color = i.second.is_auto_color ? Wittchen::WitthcenEspStyleEditor::GetAutoColor(i.second.text, ply) : i.second.color;
		i.second.text = format_text_for_entity(i.second.text, ply);
	}
}

void format_esp_map_for_entities(std::unordered_map<uint64_t, esp::esp_text_t>& t, c_base_entity* ent) {
	for (auto& i : t) {
		i.second.color = i.second.is_auto_color ? Wittchen::WitthcenEspStyleEditor::GetAutoColor(i.second.text, ent) : i.second.color;
		i.second.text = format_text_for_entity(i.second.text, ent);
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
		auto text_size = render_system::fonts::tahoma_font[2]->CalcTextSizeA(font_size, FLT_MAX, 0.f, text.text.c_str());
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
		auto text_size = render_system::fonts::tahoma_font[2]->CalcTextSizeA(font_size, FLT_MAX, 0.f, text.text.c_str());
		ImVec2 base_position = { box_size.x + text_size.y / 2.f, 0 };
		ImVec2 position = { base_position.x, last_position.y != -1.f ? last_position.y + text_size.y : base_position.y };

		last_position = position;
		return position;
	}

	if (text.relative_position == (int)esp::e_esp_text_position::left) {
		auto& last_position = last_positions[(int)esp::e_esp_text_position::left];
		auto font_size = (text.size == -1 || text.size == 0) ? calc_font_size(box) : text.size;
		auto text_size = render_system::fonts::tahoma_font[2]->CalcTextSizeA(font_size, FLT_MAX, 0.f, text.text.c_str());
		ImVec2 base_position = { -text_size.x - (text_size.y / 2.f), 0 };
		ImVec2 position = { base_position.x, last_position.y != -1.f ? last_position.y + text_size.y : base_position.y };

		last_position = position;
		return position;
	}

	if (text.relative_position == (int)esp::e_esp_text_position::down) {
		auto& last_position = last_positions[(int)esp::e_esp_text_position::down];
		auto font_size = (text.size == -1 || text.size == 0) ? calc_font_size(box) : text.size;
		auto text_size = render_system::fonts::tahoma_font[2]->CalcTextSizeA(font_size, FLT_MAX, 0.f, text.text.c_str());
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

void render_strings(esp::c_esp_box& box, c_base_entity* ent) {
	//format strings first
	if (ent->is_player())
		format_esp_map_for_players(box.text_storage.strings, (c_base_player*)ent);
	else
		format_esp_map_for_entities(box.text_storage.strings, ent);

	//constants
	box.text_storage.last_positions.fill({ -1, -1 });

	//render strings
	{
		for (auto& i : box.text_storage.strings) {
			if (i.second.text.empty() || i.second.text[0] == '%') continue;

			auto position = esp::c_esp_box::calc_text_position(box, i.second, box.text_storage.last_positions);
			const auto font_size = (i.second.size == -1 || i.second.size == 0) ? calc_font_size(box) : i.second.size;
			directx_render::text(render_system::fonts::tahoma_font[2], i.second.text, box.get_screen_position(position), font_size, i.second.color, i.second.flags);
		}
	}
}



void esp::draw_esp() {
	
	
	if (!interfaces::engine->is_in_game())
		return;
	if (globals::panic)
		return;
	for (auto i : game_utils::get_valid_entities(true)) {
		auto p = get_player_by_index(i);

		

		c_esp_box box;
		if (!c_esp_box::calc_box(p, box))
			continue;

		Wittchen::ApplyStyleToBox(box);

		if (p->is_player())
		{
			if (p->is_dormant() && settings::get_bool("esp_players_dormant"))
				continue;
			if (get_local_player()->get_eye_pos().distance_to(p->get_eye_pos()) > settings::get_float("esp_dist"))
				continue;
				render_strings(box, p);
				auto sid = p->get_steam_id();
				if (!sid.empty())
				{
					box.colorbox = p->get_team_color();
					if (settings::get_bool("esp_player_enable")) {
						switch ((box_type)box.type) {

						case box_type::border:
							directx_render::bordered_rect({ box.min.x - 1.f, box.min.y - 1.f }, { box.max.x + 1.f, box.max.y + 1.f }, box.border_color, box.rounding);
							directx_render::bordered_rect({ box.min.x + 1.f, box.min.y + 1.f }, { box.max.x - 1.f, box.max.y - 1.f }, box.border_color, box.rounding);
							directx_render::bordered_rect(box.min, box.max, (std::find(globals::friends.begin(), globals::friends.end(), sid) != globals::friends.end()) ? box.friendcolor : box.colorbox, box.rounding);
							break;
						case box_type::corner:
							directx_render::corner_box(box.min, box.max, (std::find(globals::friends.begin(), globals::friends.end(), sid) != globals::friends.end()) ? box.friendcolor : box.colorbox);
						}
					}
				}
			
		}
		else if (!p->is_player()&&globals::entitys_to_draw.exist(p->get_classname()))
		{
			if (p->is_dormant() && settings::get_bool("esp_entitie_dormant"))
				continue;
				if (get_local_player()->get_eye_pos().distance_to(p->get_eye_pos()) > settings::get_float("esp_dist_ent"))
					continue;
				box.type = settings::get_int("esp_type_ent");
				box.colorbox = c_color(globals::colorespentity[0] * 255.f, globals::colorespentity[1] * 255.f, globals::colorespentity[2] * 255.f);
				render_strings(box, p);
				if (settings::get_bool("esp_entitie_enable")) {
					switch ((box_type)box.type) {
					case box_type::border:
						directx_render::bordered_rect({ box.min.x - 1.f, box.min.y - 1.f }, { box.max.x + 1.f, box.max.y + 1.f }, box.border_color, box.rounding);
						directx_render::bordered_rect({ box.min.x + 1.f, box.min.y + 1.f }, { box.max.x - 1.f, box.max.y - 1.f }, box.border_color, box.rounding);
						directx_render::bordered_rect(box.min, box.max, box.colorentity, box.rounding);
						break;
					case box_type::corner:
						directx_render::corner_box(box.min, box.max, box.colorentity);
					}
				}
			
		}
	}
}
