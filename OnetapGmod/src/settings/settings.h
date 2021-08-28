#pragma once

#include "bind_system.h"
#include <string>

namespace settings {
	using namespace bind_system;

	using var_id_t = std::string;
	using var_id_t_non_copy = const var_id_t&;
	inline bool aimbotenable;
	inline bool aimbot_autofire;
	inline uint32_t fakelagkey;
	inline bool glua;
	inline uint32_t aimbotkey;
	bool& get_bool(var_id_t_non_copy name);
	float& get_float(var_id_t_non_copy name);
	int& get_int(var_id_t_non_copy name);

	void set_bool(var_id_t_non_copy, bool val);
	void set_float(var_id_t_non_copy, float val);
	void set_int(var_id_t_non_copy, int val);

	bool is_bool(var_id_t_non_copy s);
	bool is_int(var_id_t_non_copy s);
	bool is_float(var_id_t_non_copy s);
}
