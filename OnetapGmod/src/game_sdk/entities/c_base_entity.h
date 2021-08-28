#pragma once
#include "color.h"
#include "../../utils/netvars.h"
#include "../../interfaces.h"

#include "../../utils/memoryutils.h"

#include "i_client_entity.h"

#include <map>

#define CHECK_THIS if (!this) return

class c_collidable
{
public:
	virtual void unknown_0() = 0;
	virtual c_vector& mins() = 0;
	virtual c_vector& maxs() = 0;
};

static inline std::map<std::string, std::vector<std::string>> entity_bones_by_class {
	{"head", {"ValveBiped.Bip01_Head", "ValveBiped.Bip01_Neck1"}},
	{"pelvis", {"ValveBiped.Bip01_Pelvis"}},
	{"body", {"ValveBiped.Bip01_Spine", "ValveBiped.Bip01_Spine2",
			"ValveBiped.Bip01_Spine3", "ValveBiped.Bip01_Spine4"}},
	{"arm", {"ValveBiped.Bip01_L_UpperArm", "ValveBiped.Bip01_L_Forearm", "ValveBiped.Bip01_L_Hand"
	         "ValveBiped.Bip01_R_UpperArm", "ValveBiped.Bip01_R_Forearm", "ValveBiped.Bip01_R_Hand"}},
	{"foot", {"ValveBiped.Bip01_L_Thigh", "ValveBiped.Bip01_L_Calf", "ValveBiped.Bip01_L_Foot", "ValveBiped.Bip01_L_Toe0",
			  "ValveBiped.Bip01_R_Thigh", "ValveBiped.Bip01_R_Calf", "ValveBiped.Bip01_R_Foot", "ValveBiped.Bip01_R_Toe0"}}
};

class c_base_entity : public i_client_entity
{
public:
	NETVAR("DT_BaseEntity", "m_vecOrigin", get_origin, c_vector);
	NETVAR("DT_GMOD_Player", "m_angEyeAngles[0]", get_angles, c_vector);
	NETVAR("DT_BaseEntity", "m_nModelIndex", get_model_index, int);
	NETVAR("DT_BaseEntity", "m_iTeamNum", get_team_num, int);
	NETVAR("DT_BasePlayer", "m_vecViewOffset[0]", get_view_offset, c_vector);
	NETVAR("DT_BaseEntity", "m_flAnimTime", get_anim_time, float);
	NETVAR("DT_BaseEntity", "m_flSimulationTime", get_simulation_time, float);
	NETVAR("DT_BaseEntity", "m_angRotation", get_rotation, c_vector);
	NETVAR("DT_BaseEntity", "m_hOwnerEntity", get_owner_entity_handle, uintptr_t);
	NETVAR("DT_BaseEntity", "m_iHealth", get_health, int);
	NETVAR("DT_BaseEntity", "m_iMaxHealth", get_max_health, int);

	c_vector get_eye_pos() { return this->get_origin() + this->get_view_offset(); }

	c_collidable* get_collidable_ptr()
	{
		using original_fn = c_collidable * (__thiscall*)(void*);
		return (*(original_fn**)this)[3](this);
	}

	bool is_alive()
	{
		using original_fn = bool(__fastcall*)(void*);
		return (*(original_fn**)this)[129](this);
	}

	bool is_player()
	{
		using original_fn = bool(__fastcall*)(void*);
		return (*(original_fn**)this)[130](this);
	}

	void push_entity() //https://imgur.com/bK6cNdu
	{
		using original_fn = void(__fastcall*)(void*);
		return (*(original_fn**)this)[172](this);
	}

	bool is_use_lua()
	{
		using fn = bool(__fastcall*)(void*);
		return (*(fn**)this)[170](this);
	}

	const char* get_lua_script_name() //B8 AA 2A 4D 00 C3	client.dll
	{
		using fn = const char* (__fastcall*)(void*);
		return (*(fn**)this)[183](this);
	}

	//E8 ? ? ? ? F3 0F 11 BD ? ? ? ?
	void set_abs_origin(const c_vector& position)
	{
		using fn = void(__fastcall*)(void*, const c_vector& origin);
		static fn orig_fn;
		if (!orig_fn)
			orig_fn = reinterpret_cast<fn>(memory_utils::relative_to_absolute((uintptr_t)memory_utils::pattern_scanner("client.dll",
				"E8 ? ? ? ? F3 0F 11 BD ? ? ? ?"), 0x1, 5));
		orig_fn(this, position);
	}


	void set_abs_angles(const c_vector& ang)
	{
		using fn = void(__fastcall*)(void*, const c_vector& origin);
		static fn orig_fn;
		if (!orig_fn)
			orig_fn = reinterpret_cast<fn>(memory_utils::relative_to_absolute((uintptr_t)memory_utils::pattern_scanner("client.dll",
				"E8 ?? ?? ?? ?? 48 8D 57 70"), 0x1, 5));
		orig_fn(this, ang);
	}

	std::string get_classname() {
		using fn = const char* (__fastcall*)(void*);
		static fn f;
		if (!f)
			f = reinterpret_cast<fn>(memory_utils::relative_to_absolute(
				(uintptr_t)memory_utils::pattern_scanner("client.dll", "E8 ? ? ? ? 4D 8B 47 10"), 0x1, 5));
		return f(this);
	}
	
	bool is_equal(c_base_entity* ent) const {
		return ent->get_index() == get_index();
	}

	float get_health_procentage() {
		return get_health() / (get_max_health() / 100.f);
	}

	uint32_t get_bone_by_name(const std::string& name)
	{
		CHECK_THIS 0;
		auto lua = interfaces::lua_shared->get_lua_interface((int)e_special::glob);
		if (!lua) return 0;
		c_lua_auto_pop p(lua);

		push_entity(); //1

		lua->get_field(-1, "LookupBone"); //2
		lua->push(-2); //3
		lua->push_string(name.c_str()); //4
		lua->call(2, 1); // 3
		return lua->get_number(-1);;
	}

	c_vector get_bone(int bone)
	{
		CHECK_THIS{};
		matrix3x4_t bone_matrix[128];
		if (!setup_bones(bone_matrix, 128, 0x00000100, interfaces::engine->get_last_time_stamp()))
			return {};
		auto hitbox = bone_matrix[bone];
		return { hitbox[0][3], hitbox[1][3], hitbox[2][3] };
	}
};

__forceinline c_base_entity* get_entity_by_index(const int i)
{
	return static_cast<c_base_entity*>(interfaces::entity_list->get_client_entity(i));
}