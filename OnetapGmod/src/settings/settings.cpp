#include "settings.h"

#include "../globals.h"

using namespace settings;

internal::settings_storage_type settings_storage {
	{"bhop", false},
	{"aimbot", false},
	{"aimbot_autofire", false},
	{"norecoil", false},
	{"nospread", false},
	{"aimbot_fov_draw", false},
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
	{"aimbot_fov", 0},
	{"esp_dist", 20000.f},
	{"aimbot_bones", (int)e_bones::head},
	{"custom_viewmodel_fov", 0},
	{"custom_fov", 0},
	{"custom_aspect_ratio", 0},
	{"third_person_distance", 50},
	{"fake_lags_amount", 0},
	{"autostrafe", false},
	{"add_entity_bind", 0}

};

internal::settings_storage_type& internal::get_settings_storage() {
	return settings_storage;
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

bool settings::is_bool(var_id_t_non_copy s) {
	return is_type<bool>(s);
}

bool settings::is_int(var_id_t_non_copy s) {
	return is_type<int>(s);
}

bool settings::is_float(var_id_t_non_copy s) {
	return is_type<float>(s);
}

bool var_exist(var_id_t_non_copy name) {
	return settings_storage.find(name) != settings_storage.end();
}