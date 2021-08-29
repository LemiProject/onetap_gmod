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
	{"esp_dist", 20000.f},
	{"aimbot_bones", (int)e_bones::head},
	{"custom_viewmodel_fov", 0},
	{"custom_fov", 0},
	{"custom_aspect_ratio", 0},
	{"third_person_distance", 50},
	{"fake_lags_amount", 0},
	{"autostrafe", false}

};

void settings::c_entity_list::push_back(const std::string& c)
{
	//std::unique_lock<std::mutex> l(mutex);
	if (!exist(c))
		classes.push_back(c);
}

void settings::c_entity_list::remove(int idx)
{
	//std::unique_lock<std::mutex> l(mutex);
	classes.erase(classes.begin() + idx);
}

bool settings::c_entity_list::exist(const std::string& c)
{
	//std::lock_guard<std::mutex> l(mutex);
	return std::find(classes.begin(), classes.end(), c) != classes.end();
}

int settings::c_entity_list::find(const std::string& c)
{
	//std::lock_guard<std::mutex> l(mutex);
	if (!classes.empty())
		for (auto i = 0; i < classes.size(); ++i)
			if (classes.at(i) == c)
				return i;

	return -1;
}

void settings::c_entity_list::exchange(const std::vector<std::string>& c)
{
	//std::unique_lock<std::mutex> l(mutex);
	classes = c;
}

bool settings::c_entity_list::empty()
{
	//std::lock_guard<std::mutex> l(mutex);
	return classes.empty();
}

void settings::c_entity_list::clear()
{
	classes.clear();
}

size_t settings::c_entity_list::size()
{
	return classes.size();
}

std::vector<std::string> settings::c_entity_list::data()
{
	return classes;
}
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
