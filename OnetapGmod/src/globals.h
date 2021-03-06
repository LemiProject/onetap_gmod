#pragma once

#include <Windows.h>
#include <string>
#include <iostream>
#include "game_sdk/misc/viewsetup.h"
#include "game_sdk/misc/usercmd.h"
#include <mutex>
#include <vector>
#include "../include/color.h"
#include "json.hpp"
#include "render_system/render_system.h"
#include <color.h>
#include "game_sdk/entities/c_base_player.h"
namespace globals {
	inline HINSTANCE dllInstance;

	namespace game_info {
		inline c_view_setup view_setup;
		inline c_view_setup proof_view_setup;
		inline int chocked_packets;

	}
	inline q_angle lastview;
	inline std::vector<std::string> friends;
	inline std::vector<int> friendly_teams;
	inline c_color local_color_team;
	inline bool get_target;
	struct player_t
	{
		c_vector pos;
		c_color color;
	};
	inline std::map<std::string, player_t> player_info;
	class c_entity_list
	{
		std::vector<std::string> classes;
		std::mutex mutex;

	public:
		void push_back(const std::string& c);
		void remove(int idx);
		bool exist(const std::string& c);
		int find(const std::string& c);
		void exchange(const std::vector<std::string>& c);
		bool empty();
		void clear();

		size_t size();
		std::vector<std::string> data();
	};
	enum class e_pitch
	{
		none,
		up,
		down,
		emotional,
	};
	enum class e_yaw
	{
		none,
		forward,
		backward,
	};
	
	
	inline c_entity_list entitys_to_draw;
	inline bool unload;
	inline bool aimbotenable;
	inline bool aimbot_autofire;
	inline bool glua;
	inline bool thirdtemp;
	inline c_user_cmd lastCmd;
	inline c_user_cmd lastRealCmd;
	inline c_user_cmd last_cmd;
	inline bool panic;
	inline float colorfov[4]{255,255,255,1};
	inline float colortarger[4]{ 255,255,255,1};
	inline float colorespplayer[4]{ 255,255,255,1 };
	inline float colorespentity[4]{ 255,255,255,1 };
	inline float colorfriend[4]{ 0.f, 1.f*2.5f, 0.646288f*255.f, 1.00f*255.f };
	inline std::string lua;
	
	void on_save(nlohmann::json& out);
	void on_load(const nlohmann::json& in);
}

enum class e_bones : int {
	none = (0 << 0),
	head = (1 << 0),
	body = (1 << 1),
	pelvis = (1 << 2),
	arm = (1 << 3),
	foot = (1 << 4),
	last = foot
};

inline const char* to_string(e_bones e) {
	switch (e) {
	case e_bones::none: return "";
	case e_bones::head: return "head";
	case e_bones::body: return "body";
	case e_bones::pelvis: return "pelvis";
	case e_bones::arm: return "arm";
	case e_bones::foot: return "foot";
	default: return "";
	}
}
