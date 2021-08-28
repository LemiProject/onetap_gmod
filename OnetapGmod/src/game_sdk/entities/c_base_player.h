#pragma once

#include "c_base_entity.h"

#include <algorithm>

#include "lemi_utils.h"

class c_base_combat_weapon;

enum class e_move_type
{
	none = 0,
	isometric,
	walk,
	step,
	fly,
	flygravity,
	vphysics,
	push,
	noclip,
	ladder,
	observer,
	custom,
	last = custom,
	max_bits = 4,
	movetype
};

class c_base_player : public c_base_entity {
public:
	NETVAR("DT_BasePlayer", "m_fFlags", get_flags, int);
	NETVAR("DT_BasePlayer", "m_vecViewOffset[0]", get_view_offset, c_vector);
	NETVAR("DT_BasePlayer", "m_hActiveWeapon", active_weapon_handle, uintptr_t);
	NETVAR("DT_BasePlayer", "m_vecVelocity[0]", get_velocity, c_vector);
	NETVAR("DT_BasePlayer", "hMyWeapons", get_weapons, uintptr_t);
	NETVAR("DT_BasePlayer", "m_bloodColor", get_blood_color, c_vector);
	NETVAR("DT_BasePlayer", "m_Local", get_local, uintptr_t);
	NETVAR("DT_BasePlayer", "m_iAmmo", get_ammo, int);
	NETVAR("DT_BasePlayer", "m_fOnTarget", get_on_target, int);
	NETVAR("DT_BasePlayer", "m_nTickBase", get_tick_base, int);
	NETVAR("DT_BasePlayer", "m_nNextThinkTick", get_next_think_tick, float);
	NETVAR("DT_BasePlayer", "m_flFriction", get_friction, float);
	NETVAR("DT_BasePlayer", "flDeathTime", get_death_time, float);
	NETVAR("DT_BasePlayer", "deadflag", get_dead_flag, int);
	NETVAR("DT_BasePlayer", "m_iDefaultFOV", get_default_fov, int);
	NETVAR("DT_BasePlayer", "m_hVehicle", get_vehicle_handle, uintptr_t);
	NETVAR("DT_BasePlayer", "m_iObserverMode", get_observer_mode, int);
	NETVAR("DT_BasePlayer", "m_hObserverTarget", get_observer_target_handle, uintptr_t);
	NETVAR("DT_PlayerResource", "m_iPing", get_ping, int);

	int get_move_type()
	{
		CHECK_THIS{};
		auto glua = interfaces::lua_shared->get_lua_interface((int)e_interface_type::client);
		if (!glua)
			return {};
		c_lua_auto_pop p(glua);

		push_entity();

		glua->get_field(-1, "GetMoveType");
		glua->push(-2);
		glua->call(1, 1);

		return static_cast<int>(glua->get_number(-1));;
	}

	std::string get_name() const
	{
		player_info_s info;
		interfaces::engine->get_player_info(get_index(), &info);
		return info.name;
	}

	bool is_admin()
	{
		auto str = get_user_group();
		lowercase(str);
		return str.find("admin") != std::string::npos || str.find("owner") != std::string::npos
			|| str.find("king") != std::string::npos || str.find("moder") != std::string::npos || str.find("root") != std::string::npos;
	}

	std::string get_team_name()
	{
		CHECK_THIS{};
		auto glua = interfaces::lua_shared->get_lua_interface((int)e_special::glob);
		if (!glua) return {};
		c_lua_auto_pop p(glua);

		glua->push_special((int)e_special::glob);
		glua->get_field(-1, "team");
		glua->get_field(-1, "GetName");
		glua->push_number(this->get_team_num());
		glua->call(1, 1);

		return glua->get_string();
	}

	q_angle get_view_punch_angles()
	{
		CHECK_THIS{};
		auto lua = interfaces::lua_shared->get_lua_interface((int)e_special::glob);
		if (!lua) return { 0.f };
		lua->push_special((int)e_special::glob); //1
		push_entity();

		lua->get_field(-1, "GetViewPunchAngles");
		lua->push(-2);
		lua->call(1, 1);

		lua->push_string("x");
		lua->get_table(-2);
		float x = lua->get_number(-1);
		lua->pop();

		lua->push_string("y");
		lua->get_table(-2);
		float y = lua->get_number(-1);
		lua->pop();

		lua->push_string("z");
		lua->get_table(-2);
		float z = lua->get_number(-1);
		lua->pop();

		lua->pop(3);

		return { x, y, z };
	}
	
	c_color get_team_color()
	{
		CHECK_THIS{};
		auto glua = interfaces::lua_shared->get_lua_interface((int)e_special::glob);
		c_color color;
		if (!glua) return c_color();
		c_lua_auto_pop p(glua);

		glua->push_special((int)e_special::glob);

		glua->push_special((int)e_special::glob);
		glua->get_field(-1, "team");
		glua->get_field(-1, "GetColor");
		glua->push_number(this->get_team_num());
		glua->call(1, 1);

		glua->push_string("r");
		glua->get_table(-2);
		int r = glua->get_number(-1);
		glua->pop();

		glua->push_string("g");
		glua->get_table(-2);
		int g = glua->get_number(-1);
		glua->pop();

		glua->push_string("b");
		glua->get_table(-2);
		int b = glua->get_number(-1);
		glua->pop();

		color.init(r, g, b);
		return color;
	}
	
	std::string get_user_group()
	{
		CHECK_THIS{};
		auto lua = interfaces::lua_shared->get_lua_interface((int)e_special::glob);
		if (!lua) return {};
		c_lua_auto_pop p(lua);

		push_entity();
		lua->get_field(-1, "GetUserGroup");
		lua->push(-2);
		lua->call(1, 1);
		return lua->get_string();
	}

	void client_print(int type, const std::string& msg) {
		using fn = void(__fastcall*)(void*, int, const char*);
		static fn f;
		if (!f) {
			f = (fn)memory_utils::pattern_scanner("client.dll", "4D 85 C0 0F 84 ? ? ? ? 48 89 5C 24 ? 57");
		}
		f(this, type, msg.c_str());
	}

	c_base_combat_weapon* get_active_weapon() {
		using fn = c_base_combat_weapon * (__fastcall*)(void*);
		static fn f;
		if (!f) {
			f = (fn)memory_utils::pattern_scanner("client.dll", "40 53 48 83 EC 20 48 3B 0D ? ? ? ?");
		}
		return f(this);
	}
};

class c_local_player : public c_base_player {
public:
};

__forceinline c_local_player* get_local_player()
{
	return reinterpret_cast<c_local_player*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
}


__forceinline c_base_player* get_player_by_index(const uint32_t i)
{
	return reinterpret_cast<c_base_player*>(interfaces::entity_list->get_client_entity(i));
}