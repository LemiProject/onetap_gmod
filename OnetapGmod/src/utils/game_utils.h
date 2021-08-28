#pragma once

#include "../game_sdk/entities/c_base_entity.h"

#include "memoryutils.h"

namespace local_player_utils {
	bool is_voice_recording();
	
}

namespace game_utils {
	bool world_to_screen(const c_vector& in, c_vector& out);

	float get_fov(const c_vector& from , const c_vector& to);
	c_vector calc_angle(const c_vector& from, const c_vector& to);
	
	std::vector<int> get_valid_players(bool dormant = false);

	void trace_view_angles(trace_t& t, const q_angle& viewangles);
	void trace_view_angles(trace_t& t, const q_angle& viewangles, i_trace_filter* filter);
	void trace_ray(trace_t& t, const c_vector& from, const c_vector& to, i_trace_filter* filter);
}

/*class c_wittchen_player {
public:
	c_vector find_best_bone_to_shoot(const c_vector& from);
};*/