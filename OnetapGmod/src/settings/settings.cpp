#include "settings.h"

#include <any>

#include "../globals.h"

using namespace settings;

constexpr int bool_idx = 0;
constexpr int int_idx = 1;

std::unordered_map<std::string, std::any> settings_storage {
	{"bhop", false},
	{"aimbot", false},
	{"aimbot_autofire", true},
	{"norecoil", false},
	{"nospread", false},
	{"aimbot_fov_draw", true},
	{"aimbot_silent", false},
	{"aimbot_draw_target", false},
	{"fixmovement", false},
	{"third_person", false},
	{"faf", false},
	{"fake_lags", false},
	{"fake_duck", false},
	{"aimbot_key",0},
	{"esp_type",0},
	{"esp_enable",false},
	{"esp_info",false},
	{"aimbot_fov", 180},
	{"aimbot_bones", (int)e_bones::head},
	{"custom_viewmodel_fov", 0},
	{"custom_fov", 0},
	{"custom_aspect_ratio", 0},
	{"third_person_distance", 50},
	{"fake_lags_amount", 0},
	{"autostrafe", false}

};

bool var_exist(var_id_t_non_copy name) {
	return settings_storage.find(name) != settings_storage.end();
}

template <typename t>
t& get_var(var_id_t_non_copy name) {
	if (!var_exist(name))
		throw std::exception(("Cannot find variable " + name).c_str());
	try {
		return *std::any_cast<t>(&settings_storage[name]);
	} catch (...) {
		throw std::exception(("Value type error. Var: " + name).c_str());
	}
}

bool& settings::get_bool(var_id_t_non_copy name) {
	return get_var<bool>(name);
}

float& settings::get_float(var_id_t_non_copy name) {
	return get_var<float>(name);
}

int& settings::get_int(var_id_t_non_copy name) {
	return get_var<int>(name);
}

void settings::set_bool(var_id_t_non_copy n, bool val) {
	settings_storage[n] = val;
}

void settings::set_float(var_id_t_non_copy n, float val) {
	settings_storage[n] = val;
}

void settings::set_int(var_id_t_non_copy n, int val) {
	settings_storage[n] = val;
}

template <typename t>
bool is_type(var_id_t_non_copy s) {
	if (!var_exist(s)) return false;
	return settings_storage[s].type() == typeid(t);
}

bool settings::is_bool(var_id_t_non_copy s) {
	return is_type<bool>(s);
}

bool settings::is_int(var_id_t_non_copy s) {
	return is_type<int>(s);
}

bool settings::is_float(var_id_t_non_copy s) {
	return is_type<float>(s);
}
