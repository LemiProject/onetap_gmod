#include "input_system.h"
#include <imgui/imgui.h>

#include <map>
#include <string>
#include <vector>
#include <array>

#include <mutex>

#include <Windows.h>

#include "color.h"

std::array<bool, 1024> keys;
std::array<uint32_t, 1024> frames;
uint32_t current_frame;

std::mutex input_mutex;

void input_system::initialize_input_system() {

}

void input_system::add_bind(int64_t id, uint32_t key, bool* var, bind_system::bind_type type, bool bind_value) {
	bind_system::bool_binds[id].push_back({(int)type, key, var, bind_value});
}

void input_system::add_bind(int64_t id, uint32_t key, int* var, bind_system::bind_type type, int bind_value) {
	bind_system::int_binds[id].push_back({ (int)type, key, var, bind_value });
}

bool input_system::is_key_pressed(int key, bool null_is_true)
{
	if (key >= 1024)
		return false;
	std::unique_lock l(input_mutex);
	
	if (null_is_true)
		return keys[key] || key == 0;
	return keys[key];
}

bool input_system::is_key_just_pressed(int key, bool null_is_true)
{
	if (key >= 1024)
		return false;
	std::unique_lock l(input_mutex);
	
	if (null_is_true)
		return (keys[key] && frames[key] == current_frame) || key == 0;
	return keys[key] && frames[key] == current_frame;
}

template<typename t>
void resolve_bind(bind_system::bind_type type, uint32_t key, t& curval, t nextval, t& oldval) {
	//oldval = curval;
	switch (type) {
	case bind_system::bind_type::none:
		if (oldval != curval)
			oldval = curval;
		break;
	case bind_system::bind_type::active:
		if (curval != nextval)
			oldval = curval;
		curval = nextval;
		break;
	case bind_system::bind_type::pressed:
		if (input_system::is_key_pressed(key)) {
			if (curval != nextval)
				oldval = curval, curval = nextval;
		} else {
			if (curval != oldval)
				curval = oldval;
			curval = oldval;
		}
		break;
	case bind_system::bind_type::toggle:
		if (input_system::is_key_just_pressed(key)) {
			if (curval != nextval)
				oldval = curval, curval = nextval;
			else
				curval = oldval;
		}
		break;
	default:
		curval = oldval;
		break;
	}
}

void input_system::process_binds() {
	//
	
	for (auto& i : bind_system::bool_binds)
		for (auto& j : i.second)
			resolve_bind<bool>((bind_system::bind_type)j.type, j.key, *j.value, j.bind_value, j.old_value);

	for (auto& i : bind_system::float_binds)
		for (auto& j : i.second)
			resolve_bind<float>((bind_system::bind_type)j.type, j.key, *j.value, j.bind_value, j.old_value);

	for (auto& i : bind_system::int_binds)
		for (auto& j : i.second)
			resolve_bind<int>((bind_system::bind_type)j.type, j.key, *j.value, j.bind_value, j.old_value);

	for (auto& i : bind_system::color_binds)
		for (auto& j : i.second)
			resolve_bind<c_color>((bind_system::bind_type)j.type, j.key, *j.value, j.bind_value, j.old_value);
}

void input_system::on_windpoc(int msg, int wparam, int lparam)
{
	std::unique_lock l(input_mutex);

	current_frame++;

	if (msg == WM_SYSKEYDOWN || msg == WM_KEYDOWN) {
		if (wparam < 256) {
			keys[wparam] = true;
			
			frames[wparam] = frames[wparam] != current_frame ? current_frame : frames[wparam];
		}
	}
	else if (msg == WM_SYSKEYUP || msg == WM_KEYUP) {
		if (wparam < 256) {
			keys[wparam] = false;
			frames[wparam] = frames[wparam] != current_frame ? current_frame : frames[wparam];
		}
	}

	if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK || msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK || msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK || msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK)
	{
		int button = 0;
		if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) button = 1;
		if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) button = 2;
		if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) button = 4;
		if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK)
			button = (HIWORD(wparam) == XBUTTON1) ? 5 : 6;
		keys[button] = true;
		frames[button] = current_frame;
	}
	if (msg == WM_XBUTTONUP || msg == WM_LBUTTONUP || msg == WM_RBUTTONUP || msg == WM_MBUTTONUP) {
		int button = 0;
		if (msg == WM_LBUTTONUP ) button = 1;
		if (msg == WM_RBUTTONUP) button = 2;
		if (msg == WM_MBUTTONUP) button = 4;
		if (msg == WM_XBUTTONUP)
			button = (HIWORD(wparam) == XBUTTON1) ? 5 : 6;
		keys[button] = false;
		frames[button] = current_frame;
	}
}
