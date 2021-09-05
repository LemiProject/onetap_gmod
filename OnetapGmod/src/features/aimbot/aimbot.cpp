#include "aimbot.h"

#include <iostream>

#include "spreads.h"

#include "../../game_sdk/entities/c_base_weapon.h"
#include "../../settings/settings.h"
#include "../../utils/game_utils.h"
#include "../../globals.h"
#include "../../game_sdk/misc/movedata.h"

#include"../../render_system/render_system.h"
#include "../../utils/md5_check_sum.h"

struct shoot_pos_t {
	c_vector position;
	q_angle angle;
	float fov;
};

struct target_t {
	c_base_player* ply;
	shoot_pos_t shoot_pos;
	int idx;
};

int last_target_id = -1;
float last_target_time = -1.f;

bool can_do_damage(c_base_player* player, const c_vector& position) {
	trace_t tr;
	aimbot::c_aimbot_trace_filter filter;
	filter.lp = get_local_player();
	filter.ent = player;
	
	game_utils::trace_ray(tr, get_local_player()->get_eye_pos(), position, &filter);

	return tr.fraction == 1.f;
}

bool on_bone_not_exist(shoot_pos_t& sp, c_base_player* player) {
	auto position = player->get_origin() + ((player->get_collidable_ptr()->mins() + player->get_collidable_ptr()->maxs()) / 2.f);
	if (!position.is_valid())
		return false;
	q_angle real_angle;
	interfaces::engine->get_view_angles(real_angle);
	
	sp.fov = game_utils::get_fov(real_angle, game_utils::calc_angle(get_local_player()->get_eye_pos(), position));
	auto angle = math::get_angle(get_local_player()->get_eye_pos(), position);
	if (!angle.is_valid() || !can_do_damage(player, position))
		return false;
	sp.angle = angle;
	sp.position = position;
	
	return true;
	//return false;
}

bool get_shoot_pos(shoot_pos_t& sp, c_base_player* player) {
	int32_t bone_idx = -1;
	float best_fov = FLT_MAX;

	int bones = settings::get_int("aimbot_bones");

	if (bones == (int)e_bones::none) return on_bone_not_exist(sp, player);

	q_angle real_angle;
	interfaces::engine->get_view_angles(real_angle);
	
	for (const auto& i : entity_bones_by_class) {
		if ((i.first == "head" && !(bones & (int)e_bones::head)) || (i.first == "arm" && !(bones & (int)e_bones::arm))
			|| (i.first == "foot" && !(bones & (int)e_bones::foot)) || (i.first == "pelvis" && !(bones & (int)e_bones::pelvis)) || (i.first == "body" && !(bones & (int)e_bones::body)))
			continue;

		for (const auto& j : i.second) {
			auto idx = player->get_bone_by_name(j);
			if (idx <= 0) continue;
			c_vector position = player->get_bone(idx);
			if (!position.is_valid()) continue;
			if (const auto fov = game_utils::get_fov(real_angle, game_utils::calc_angle(get_local_player()->get_eye_pos(), position)); fov < best_fov && can_do_damage(player, position))
				best_fov = fov, bone_idx = idx;
		}
	}

	if (bone_idx == -1)
		return on_bone_not_exist(sp, player);

	const auto bone_position = player->get_bone(bone_idx);
	sp.position = bone_position;
	sp.angle = math::get_angle(get_local_player()->get_eye_pos(), sp.position);
	sp.fov = game_utils::get_fov(real_angle, game_utils::calc_angle(get_local_player()->get_eye_pos(), bone_position));

	return true;
}

bool get_target(target_t& target)
{
	auto invalidate_target = [](const target_t& t)
	{
		return (t.ply != nullptr && t.shoot_pos.angle.x != 0 && t.shoot_pos.angle.y != 0 && t.shoot_pos.fov >= 0.f && t.shoot_pos.position.is_valid()); //fov maybe FLT_MAX and 0
	};

	if (!get_local_player())
		return false;

	target_t tmp{ nullptr,{}};
	tmp.shoot_pos.fov = FLT_MAX;
	
	for (auto i : game_utils::get_valid_players(false)) {
		auto player = get_player_by_index(i);
		if(player->is_player()) {
			shoot_pos_t tmp_shoot_pos;

			if (!get_shoot_pos(tmp_shoot_pos, player))
				continue;

			if (tmp_shoot_pos.fov < tmp.shoot_pos.fov && tmp_shoot_pos.fov <= (float)settings::get_int("aimbot_fov")) {
				if (std::find(globals::friends.begin(), globals::friends.end(), player->get_steam_id()) != globals::friends.end())
					continue;
				if (std::find(globals::friendly_teams.begin(), globals::friendly_teams.end(), player->get_team_num()) != globals::friendly_teams.end())
					continue;

				tmp.ply = player;
				tmp.idx = i;
				tmp.shoot_pos = tmp_shoot_pos;
			}
		}
	}

	if (!invalidate_target(tmp))
		return false;

	target = tmp;
	return true;
}

c_vector aimbot::get_aimbot_target()
{
	target_t target;
	if (!get_target(target))
		return {};
	return target.shoot_pos.position;
}

q_angle do_smooth(const q_angle& from, const q_angle& to, float smooth_val) {
	auto delta = to - from;
	delta.normalize();
	delta /= q_angle(smooth_val, smooth_val, 0);
	auto out = to - delta;
	out.normalize();
	return out;
}

void aimbot::run_aimbot(c_user_cmd& cmd) {
	if (!globals::aimbotenable)
		return;
	if (!GetAsyncKeyState(globals::aimbotkey) && globals::aimbotkey != 0)
		return;
	if (!cmd.is_in_fire() && !settings::get_bool("aimbot_autofire"))
		return;
	
	target_t target;
	
	if (!get_target(target))
		return;

	last_target_id = target.idx;
	last_target_time = interfaces::engine->get_time_stamp_from_start();

	if (settings::get_int("aim_smooth") > 0)
		target.shoot_pos.angle = do_smooth(cmd.viewangles, target.shoot_pos.angle, settings::get_int("aim_smooth") / 100.f);
	
	cmd.viewangles = target.shoot_pos.angle;

	if (!settings::get_bool("aimbot_silent"))
		interfaces::engine->set_view_angles(cmd.viewangles);
	
	if (settings::get_bool("aimbot_autofire")) {
		cmd.buttons |= IN_ATTACK;
	}
}

void aimbot::norecoil(c_user_cmd& cmd) {
	if (!settings::get_bool("norecoil"))
		return;
	
	if (cmd.buttons & IN_ATTACK)
		cmd.viewangles -= get_local_player()->get_view_punch_angles();
}

void aimbot::nospread(c_user_cmd& cmd) {
	if (!settings::get_bool("nospread"))
		return;
	
	auto wep = get_local_player()->get_active_weapon();

	if (cmd.buttons & IN_ATTACK) {
		if (wep->get_weapon_base().find("bobs_gun") != std::string::npos)
			spreads::base_nospread(cmd);
		if (wep->get_weapon_base().find("swb") != std::string::npos)
			spreads::swb_nospread(cmd);
		//if (wep->get_classname().find("fas2") != std::string::npos)
			//spreads::fas2_nospread(cmd);
	}
}


struct prediton_data_t {
	float old_curtime, old_frametime;
	c_base_player* player;
	c_move_data move_data = {};
	c_user_cmd* current_cmd;
	int* prediction_random_seed;
} pred_data;

bool aimbot::start_prediction(c_user_cmd& cmd)
{
	/*if (!pred_data.prediction_random_seed) {
		static DWORD oldp;
		auto fn = memory_utils::relative_to_absolute((uintptr_t)memory_utils::pattern_scanner("client.dll", "E8 ? ? ? ? 48 8B 06 48 8D 55 D7"), 1, 5);
		pred_data.prediction_random_seed = (int*)(fn + 15);
		VirtualProtect(pred_data.prediction_random_seed, sizeof(int), PAGE_EXECUTE_READWRITE, &oldp);
	}

	pred_data.player = get_local_player();
	*pred_data.prediction_random_seed = md5::md5_pseudo_random(cmd.command_number) & 0x7FFFFFFF;

	pred_data.old_curtime = interfaces::global_vars->curtime;
	pred_data.old_frametime = interfaces::global_vars->interval_per_tick;

	interfaces::game_movement->start_track_prediction_errors(pred_data.player);
	pred_data.move_data.zero();
	interfaces::prediction->setup_move(pred_data.player, &cmd, interfaces::move_helper, &pred_data.move_data);
	interfaces::game_movement->process_movement(pred_data.player, &pred_data.move_data);
	interfaces::prediction->finish_move(pred_data.player, &cmd, &pred_data.move_data);*/
	
	return true;
}

void aimbot::end_prediction()
{
	/*interfaces::game_movement->finish_track_prediction_errors(pred_data.player);
	*pred_data.prediction_random_seed = -1;
	pred_data.current_cmd = nullptr;
	interfaces::global_vars->curtime = pred_data.old_curtime;
	interfaces::global_vars->frametime = pred_data.old_frametime;*/
}
