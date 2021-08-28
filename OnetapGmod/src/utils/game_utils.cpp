#include "game_utils.h"

#include <iostream>

#include "../game_sdk/entities/c_base_player.h"

bool local_player_utils::is_voice_recording() {
	using fn = bool(__thiscall*)();
	static fn func;
	if (!func) {
		func = (fn)memory_utils::pattern_scanner("engine.dll", "80 3D ? ? ? ? ? 74 0C 80 3D ? ? ? ? ?");
	}

	return func();
}

bool game_utils::world_to_screen(const c_vector& in, c_vector& out) {
	return !interfaces::debug_overlay->screen_position(in, out);
}

float game_utils::get_fov(const c_vector& from, const c_vector& to) {
	auto delta = to - from;
	delta.normalize();
	return sqrtf(powf(delta.x, 2) + powf(delta.y, 2));
}

c_vector game_utils::calc_angle(const c_vector& from, const c_vector& to) {
	c_vector ang;
	const auto delta = from - to;
	const auto hyp = delta.length2d();

	ang.y = std::atanf(delta.y / delta.x) * 57.295779513082f;
	ang.x = std::atanf(-delta.z / hyp) * -57.295779513082f;

	if (delta.x >= 0.f)
		ang.y += 180.f;

	return ang;
}

std::vector<int> game_utils::get_valid_players(bool dormant) {
	if (!interfaces::engine->is_in_game())
		return {};

	auto local_player = get_local_player();
	if (!local_player)
		return {};
	
	std::vector<int> c;
	for (auto i = 0; i < interfaces::entity_list->get_highest_entity_index(); ++i) {
		auto ent = get_entity_by_index(i);
		if (ent && ent->is_player() && ent->is_alive() /*&& (!dormant ? !ent->is_dormant() : true)*/ && !local_player->is_equal(ent)) c.push_back(i);
	}
	return c;
}

void game_utils::trace_view_angles(trace_t& t, const q_angle& viewangles) {
	if (!get_local_player() || !get_local_player()->is_alive())
		return;

	c_vector dir;
	math::angle_to_vector(viewangles, dir);

	ray_t ray;
	trace_t tr;
	c_trace_filter filter;
	filter.pSkip = get_local_player();

	tr.startpos = get_local_player()->get_eye_pos();
	tr.endpos = tr.startpos + (dir * (4096 * 8));

	ray.init(tr.startpos, tr.endpos);
	interfaces::engine_trace->trace_ray(ray, MASK_SHOT, &filter, &tr);

	t = tr;
}

void game_utils::trace_view_angles(trace_t& t, const q_angle& viewangles, i_trace_filter* filter) {
	if (!get_local_player() || !get_local_player()->is_alive())
		return;

	c_vector dir;
	math::angle_to_vector(viewangles, dir);

	ray_t ray;
	trace_t tr;

	tr.startpos = get_local_player()->get_eye_pos();
	tr.endpos = tr.startpos + (dir * (4096 * 8));

	ray.init(tr.startpos, tr.endpos);
	interfaces::engine_trace->trace_ray(ray, MASK_SHOT, filter, &tr);

	t = tr;
}

void game_utils::trace_ray(trace_t& t, const c_vector& from, const c_vector& to, i_trace_filter* filter) {
	ray_t ray;

	c_vector start = from;
	c_vector end = to;
	
	ray.init(start, end);
	interfaces::engine_trace->trace_ray(ray, MASK_SHOT, filter, &t);
}
