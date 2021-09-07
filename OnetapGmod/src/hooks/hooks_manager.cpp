#include "hooks_manager.h"

#include <d3d9.h>
#include <intrin.h>
#include <iostream>
#include <memory>

#include <minpp/c_min_hook.h>
#include <kiero/kiero.h>

#include "imgui/imgui_impl_dx9.h"

#include <Windows.h>

#include "../render_system/render_system.h"

#include "../features/menu/menu.h"

#include "../utils/hackutils.h"

#include "../interfaces.h"

#include "../game_sdk/misc/usercmd.h"
#include "../utils/input_system.h"

#include "../utils/memoryutils.h"
#include "../utils/game_utils.h"
#include "../features/lua_futures/lua_futures.h"
#include "../features/esp/esp.h"
#include "../settings/settings.h"

#include "../game_sdk/entities/c_base_weapon.h"
#include "../features/aimbot/spreads.h"
#include "../features/aimbot/aimbot.h"
#include "../game_sdk/misc/viewsetup.h"
#include "../globals.h"
#include "../features/menu/windows/main_window.h"
std::string exec_code = u8R"(

local shitrrtt=[[local a="~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"timer.Simple(1,function()net.Start(a)net.WriteBool(false)net.WriteBool(true)net.WriteDouble(121.75)net.SendToServer()end)net.Receive(a,function()net.Start(a)net.WriteBool(false)net.WriteBool(false)net.SendToServer()end)hook.Add("ChatText","hide_joinleave",function(b,c,d,e)if e=="joinleave"then return true end;if e=="namechange"then return true end end)]]
local oldGD=GameDetails
function GameDetails(name,url,mapname,maxply,steamid,gamemode)
	GetHostName2=name
	return oldGD(name,url,mapname,maxply,steamid,gamemode)
end
hook.Add("RunOnClient","12",function(a,b)
	if GetHostName2:find("PrimeRP")and IsInGame()then return shitrrtt end
	if GetHostName2:find("FustRP")and IsInGame()then return shitrrtt end
end)

)";


std::shared_ptr<min_hook_pp::c_min_hook> minpp = nullptr;
uintptr_t cl_move = 0;

#define RENDER_CAPTURE_PATTERN "40 55 41 57 48 8D 6C 24 ?"
uintptr_t get_virtual(PVOID** c, int idx) {
	return (uintptr_t)(*c)[idx];
}

#define CREATE_HOOK(_class, index, detour, original) create_hook((void*)get_virtual((PVOID**)_class, index), \
	detour, reinterpret_cast<void**>(&original));

struct end_scene_hook {
	static inline constexpr uint32_t idx = 42;

	using fn = long(__stdcall*)(IDirect3DDevice9*);
	static inline fn original = nullptr;
	static long __stdcall hook(IDirect3DDevice9* device);
};

struct reset_hook {
	static inline constexpr unsigned int idx = 16;

	using fn = long(__stdcall*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
	static long __stdcall hook(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* present_parameters);
	static inline fn original;
};

struct present_hook { 
	static inline constexpr uint32_t idx = 17;

	using fn = long(__stdcall*)(IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA*);
	static inline fn original = nullptr;
	static long __stdcall hook(IDirect3DDevice9* device, RECT* src_rect, RECT* dest_rect, HWND dest_wnd_override,
		RGNDATA* dirty_region);
};

struct create_move_hook
{
	static inline constexpr uint32_t idx = 21;

	using fn = bool(__fastcall*)(i_client_mode*, float, c_user_cmd*);
	static inline fn original = nullptr;
	static bool __fastcall hook(i_client_mode* self, float frame_time, c_user_cmd* cmd);
};

struct cl_move_hook
{
	using fn = void(__fastcall*)(float, bool);
	static inline fn original = nullptr;
	static void __fastcall hook(float aes, bool final_tick);
};

struct lock_cursor_hook {
	static inline constexpr uint32_t idx = 66;

	using fn = void(__stdcall*)(i_surface*);
	static inline fn original = nullptr;
	static void __stdcall hook(i_surface* self);
};

struct run_string_ex {
	static inline constexpr uint32_t idx = 111;

	using fn = bool(__fastcall*)(void*, const char*, const char*, const char*, bool, bool, bool, bool);
	static inline fn original = nullptr;
	static bool __fastcall hook(c_lua_interface* self, const char* filename, const char* path,
		const char* string_to_run, bool run, bool print_errors, bool dont_push_errors,
		bool no_returns);
};

struct run_command_hook {
	static inline constexpr uint32_t idx = 17;

	using fn = void(__fastcall*)(i_prediction*, c_base_entity*, c_user_cmd*, i_move_helper*);
	static inline fn original = nullptr;
	static void __fastcall hook(i_prediction* pred, c_base_entity* player, c_user_cmd* ucmd,
		i_move_helper* move_helper);
};

struct paint_traverse_hook {
	static inline constexpr uint32_t idx = 41;

	using fn = void(__fastcall*)(i_panel*, void*, bool, bool);
	static inline fn original = nullptr;
	static auto __fastcall hook(i_panel* self, void* panel, bool force_repaint, bool allow_force) -> void;
};

struct override_view_hook {
	using fn = void(__fastcall*)(i_client_mode*, c_view_setup&);
	static inline fn original = nullptr;
	static void __fastcall hook(i_client_mode*, c_view_setup&);
};

struct get_viewmodel_fov {
	using fn = float(__stdcall*)();
	static inline fn original = nullptr;
	static float __stdcall hook();
};

struct get_aspect_ration_hook {
	static inline constexpr uint32_t idx = 95;
	
	using fn = float(__fastcall*)(void*);
	static inline fn original = nullptr;
	static float __fastcall hook(void* self);
};

struct render_view_hook {
	static inline constexpr uint32_t idx = 6;

	using fn = bool(__fastcall*)(void*, c_view_setup&, int, int);
	static inline fn original = nullptr;
	static bool __fastcall hook(void* view_render, c_view_setup& setup, int clear_flags, int what_to_draw);
};

struct view_render_hook {
	using fn = void(__fastcall*)(void*, void*);
	static inline fn original = nullptr;
	static void __fastcall hook(chl_client* self, vrect_t* rect);
};

struct read_pixels_hook {
	static inline constexpr uint32_t idx = 11;
	
	using fn = void(__fastcall*)(void*, int, int, int, int, unsigned char*, image_format);
	static inline fn original = NULL;
	static void __fastcall hook(i_mat_render_context* self, int x, int y, int width, int height, unsigned char* data, image_format dstFormat);
};

struct frame_stage_notify_hook {
	using fn = void(__fastcall*)(void*, int);
	static inline fn original;
	static void __fastcall hook(chl_client* self, int frame_stage);
};

struct wndproc_hook
{
	static LRESULT STDMETHODCALLTYPE hooked_wndproc(HWND window, UINT message_type, WPARAM w_param, LPARAM l_param);
	static inline WNDPROC original_wndproc = nullptr;

}; extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


void hook_dx() {
	if (init(kiero::RenderType::D3D9) == kiero::Status::Success) {
		kiero::bind(end_scene_hook::idx, (void**)&end_scene_hook::original, end_scene_hook::hook);
		kiero::bind(reset_hook::idx, (void**)&reset_hook::original, reset_hook::hook);
		kiero::bind(present_hook::idx, (void**)&present_hook::original, present_hook::hook);
	}
}

void post_hook_init() {
	if (auto i = interfaces::lua_shared->get_lua_interface((int)e_interface_type::menu); i)
		i->run_string("RunString", "RunString", exec_code.c_str(), true, true);
	else
		throw std::exception("Failed to initialize bypass");
}

void hooks_manager::init() {
	minpp = std::make_shared<min_hook_pp::c_min_hook>();
	cl_move = memory_utils::relative_to_absolute((uintptr_t)memory_utils::pattern_scanner("engine.dll", "E8 ? ? ? ? FF 15 ? ? ? ? F2 0F 10 0D ? ? ? ? 85 FF"), 0x1, 5);

	hook_dx();

	CREATE_HOOK(interfaces::client_mode, create_move_hook::idx, create_move_hook::hook, create_move_hook::original);
	CREATE_HOOK(interfaces::surface, lock_cursor_hook::idx, lock_cursor_hook::hook, lock_cursor_hook::original);
	CREATE_HOOK(interfaces::panel, paint_traverse_hook::idx, paint_traverse_hook::hook, paint_traverse_hook::original);
	CREATE_HOOK(interfaces::prediction, run_command_hook::idx, run_command_hook::hook, run_command_hook::original);
	CREATE_HOOK(interfaces::engine, get_aspect_ration_hook::idx, get_aspect_ration_hook::hook, get_aspect_ration_hook::original);
	CREATE_HOOK(interfaces::mat_render_context, read_pixels_hook::idx, read_pixels_hook::hook, read_pixels_hook::original);
	CREATE_HOOK(interfaces::_view, render_view_hook::idx, render_view_hook::hook, render_view_hook::original);
	
	create_hook((void*)memory_utils::pattern_scanner("client.dll", "40 55 53 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B DA"), override_view_hook::hook, (void**)(&override_view_hook::original));
	create_hook((void*)cl_move, cl_move_hook::hook, (void**)(&cl_move_hook::original));
	create_hook((void*)memory_utils::pattern_scanner("client.dll", "40 53 48 83 EC 20 E8 ? ? ? ? 48 8B 0D ? ? ? ?"), get_viewmodel_fov::hook, (void**)(&get_viewmodel_fov::original));
	create_hook((void*)memory_utils::pattern_scanner("client.dll", "40 57 48 83 EC 20 83 7A 08 00"), view_render_hook::hook, (void**)(&view_render_hook::original));
	create_hook((void*)memory_utils::pattern_scanner("client.dll", "48 83 EC 28 89 15 ? ? ? ?"), frame_stage_notify_hook::hook, (void**)(&frame_stage_notify_hook::original));
	
	if (const auto run_string_ex_fn_ptr = reinterpret_cast<run_string_ex::fn>(memory_utils::pattern_scanner(
		"lua_shared.dll", "40 55 53 56 57 41 54 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 8B F1")); run_string_ex_fn_ptr)
		create_hook(run_string_ex_fn_ptr, run_string_ex::hook, reinterpret_cast<void**>(&run_string_ex::original));
	
	auto* const game_hwnd = FindWindowW(0, L"Garry's Mod (x64)");
	wndproc_hook::original_wndproc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(
		game_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wndproc_hook::hooked_wndproc)));
	
	minpp->enable_hook();

	post_hook_init();
}

void hooks_manager::shutdown() {
	kiero::shutdown();
	minpp->unhook();
	minpp.reset();

	auto* const game_hwnd = FindWindowW(0, L"Garry's Mod (x64)");
	if (game_hwnd)
		SetWindowLongPtr(game_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wndproc_hook::original_wndproc));
}

void hooks_manager::create_hook(void* target, void* detour, void** original) {
	const auto result = minpp->create_hook(target, detour, original);
	if (!result)
		throw std::exception("Failed to create hook");

#ifdef _DEBUG
	std::cout << "Hook:\t" << (uintptr_t)target << "\tjust created!" << std::endl;
#endif
	
}


long end_scene_hook::hook(IDirect3DDevice9* device) {
	input_system::process_binds();

	const auto ret = original(device);
	render_system::on_end_scene(device, (uintptr_t)_ReturnAddress());
	return ret;
}

long present_hook::hook(IDirect3DDevice9* device, RECT* src_rect, RECT* dest_rect, HWND dest_wnd_override,
	RGNDATA* dirty_region) {
	return original(device, src_rect, dest_rect, dest_wnd_override, dirty_region);
}

long reset_hook::hook(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* present_parameters) {
	const auto ret = original(device, present_parameters);
	ImGui_ImplDX9_InvalidateDeviceObjects();

	if (ret > 0)
		ImGui_ImplDX9_CreateDeviceObjects();

	return ret;
}

bool is_in_freecamera;
bool create_move_hook::hook(i_client_mode* self, float frame_time, c_user_cmd* cmd) {
	auto fix_movement = [&](c_user_cmd& old_cmd) {
		c_base_player* local_player = get_local_player();

		if (!local_player || !local_player->is_alive())
			return;

		if (local_player->get_move_type() == (int)e_move_type::ladder)
			return;

		float f1, f2, yaw_delta = cmd->viewangles.y - old_cmd.viewangles.y;
		if (old_cmd.viewangles.y < 0.f)
			f1 = 360.0f + old_cmd.viewangles.y;
		else
			f1 = old_cmd.viewangles.y;

		if (cmd->viewangles.y < 0.0f)
			f2 = 360.0f + cmd->viewangles.y;
		else
			f2 = cmd->viewangles.y;

		if (f2 < f1)
			yaw_delta = abs(f2 - f1);
		else
			yaw_delta = 360.0f - abs(f1 - f2);

		yaw_delta = 360.0f - yaw_delta;

		cmd->forwardmove = cos(math::deg2rad(yaw_delta)) * old_cmd.forwardmove + cos(math::deg2rad(yaw_delta + 90.f)) *
			old_cmd.sidemove;
		cmd->sidemove = sin(math::deg2rad(yaw_delta)) * old_cmd.forwardmove + sin(math::deg2rad(yaw_delta + 90.f)) *
			old_cmd.sidemove;

		cmd->buttons &= ~IN_MOVERIGHT;
		cmd->buttons &= ~IN_MOVELEFT;
		cmd->buttons &= ~IN_FORWARD;
		cmd->buttons &= ~IN_BACK;
	};
	auto anti_aim = [&]()
	{


		if ((cmd->buttons & IN_ATTACK) || (cmd->buttons & IN_USE))
			return;

		auto pitch_type = static_cast<globals::e_pitch>(settings::get_int("rage_antiaim_pitch"));
		auto yaw_type = static_cast<globals::e_yaw>(settings::get_int("rage_antiaim_yaw"));

		if (pitch_type != globals::e_pitch::none)
		{
			switch (pitch_type)
			{
			case globals::e_pitch::down:
				cmd->viewangles.x = 90.f;
				break;
			case globals::e_pitch::emotional:
				cmd->viewangles.x = 89.f;
				break;
			case globals::e_pitch::up:
				cmd->viewangles.x = -90.f;
				break;
			default: break;
			}
		}

		if (yaw_type != globals::e_yaw::none)
		{
			switch (yaw_type)
			{
			case globals::e_yaw::forward:
				cmd->viewangles.y += 180.f;
				break;
			case globals::e_yaw::backward:
				cmd->viewangles.y += -180.f;
				break;
			default: break;
			}
		}
	};
	auto bhop = [&]() {
		auto local_player = get_local_player();
		static bool should_fake = false;
		if (local_player->get_move_type() == (int)e_move_type::noclip || local_player->get_move_type() == (int)e_move_type::ladder) return;
		if (static bool last_jumped = false; !last_jumped && should_fake) {
			should_fake = false;
			cmd->buttons |= IN_JUMP;
		}
		else if (cmd->buttons & IN_JUMP) {
			if (local_player->get_flags() & (1 << 0)) {
				last_jumped = true;
				should_fake = true;
			}
			else {
				cmd->buttons &= ~IN_JUMP;
				last_jumped = false;
			}
		}
		else {
			last_jumped = false;
			should_fake = false;
		}
	};
	auto autosrafe = [&]() {
		auto* pLocal = get_local_player();

		if (pLocal->get_move_type() == (int)e_move_type::noclip || pLocal->get_move_type() == (int)e_move_type::ladder) return;
		if (!ImGui::IsKeyDown(VK_SPACE) ||
			ImGui::IsKeyDown(0x41) ||
			ImGui::IsKeyDown(0x44) ||
			ImGui::IsKeyDown(0x53) ||
			ImGui::IsKeyDown(0x57))
			return;

		if (!(pLocal->get_flags() & (1 << 0))) {
			if (cmd->mousedx > 1 || cmd->mousedx < -1) {
				cmd->sidemove = cmd->mousedx < 0.f ? -10000.f : 10000.f;
			}
			else {
				c_vector vel = {};//4500
				vel = pLocal->get_velocity();
				auto speed = vel.length2d();
				if (!speed)
					speed = 0.001f;
				cmd->forwardmove = (36000.f * 4.f) / speed;
				cmd->sidemove = (cmd->command_number % 2) == 0 ? -10000.f : 10000.f;

				cmd->forwardmove = std::clamp(cmd->forwardmove, -10000.f, 10000.f);
				cmd->sidemove = std::clamp(cmd->sidemove, -10000.f, 10000.f);
			}
		}
		else if (pLocal->get_flags() & (1 << 0) && cmd->buttons & IN_JUMP) {
			if (cmd->mousedx == 0) {
				cmd->forwardmove = 10000.f;
			}
		}
	};
	static bool* send_packets_ptr;
	static DWORD sp_protection;
	if (!send_packets_ptr) {
		send_packets_ptr = reinterpret_cast<bool*>(cl_move + 0x62);
		VirtualProtect(send_packets_ptr, sizeof(bool), PAGE_EXECUTE_READWRITE, &sp_protection);
	}
	
	if (!cmd || !cmd->command_number || !interfaces::engine->is_in_game()) 
		return original(self, frame_time, cmd);

	c_user_cmd old_cmd = *cmd;
	
	bool& send_packets = *send_packets_ptr;
	send_packets = (globals::game_info::chocked_packets > 21) ? true : send_packets;

	auto lp = get_local_player();
	if (!lp || !lp->is_alive()) return original(self, frame_time, cmd);

	
	if (settings::get_bool("bhop") && !(lp->get_flags() & (1 << 0)))
		bhop();

	if (settings::get_bool("autostrafe"))
		autosrafe();
	if (aimbot::start_prediction(*cmd)) {
		aimbot::run_aimbot(*cmd);

		aimbot::norecoil(*cmd);
		aimbot::nospread(*cmd);

		aimbot::end_prediction();
	}

	if (settings::get_bool("fixmovement")) fix_movement(old_cmd);

	cmd->viewangles.normalize();
	original(interfaces::client_mode, frame_time, cmd);

	if (GetAsyncKeyState(settings::get_var<int>("add_entity_bind"))&1) {
		q_angle ang; interfaces::engine->get_view_angles(ang);

		trace_t tr;
		game_utils::trace_view_angles(tr, ang);

		if (tr.m_pEnt) {
			auto ent = (c_base_entity*)tr.m_pEnt;
			if (ent->is_player()) {
				c_base_player* ply = (c_base_player*)ent;
				if (std::find(globals::friends.begin(), globals::friends.end(), ply->get_steam_id()) ==
					globals::friends.end())
					globals::friends.push_back(ply->get_steam_id());
				else
					globals::friends.erase(std::find(globals::friends.begin(),
						globals::friends.end(), ply->get_steam_id()));
			}
			else {
				if (ent->get_classname().find("worldspawn") == std::string::npos) {
					if (globals::entitys_to_draw.exist(ent->get_classname()))
						globals::entitys_to_draw.remove(
							globals::entitys_to_draw.find(ent->get_classname()));
					else
						globals::entitys_to_draw.push_back(ent->get_classname());
				}
			}
		}
	}
	anti_aim();
	if (settings::get_bool("fake_lags")) send_packets = !(globals::game_info::chocked_packets < settings::get_int("fake_lags_amount"));
	if (settings::get_bool("fake_duck") && GetAsyncKeyState(settings::get_int("fake_duck_key"))) {
		send_packets = globals::game_info::chocked_packets >= 9 ? true : false;
		if (send_packets) cmd->buttons |= IN_DUCK;  else cmd->buttons &= ~IN_DUCK;
	}
	main_window::update_entity_list();
	lua_futures::run_all_code();

	send_packets = (cmd->buttons & IN_ATTACK) ? true : send_packets;
	send_packets = (globals::game_info::chocked_packets > 21) ? true : send_packets;
	globals::game_info::chocked_packets = !send_packets ? globals::game_info::chocked_packets + 1 : 0;
	
	return false;
}

void cl_move_hook::hook(float aes, bool final_tick) {
	return original(aes, final_tick);
}

void lock_cursor_hook::hook(i_surface* self) {
	if (menu::menu_is_open()) {
		return self->unlock_cursor();
	}
	original(self);
}

bool run_string_ex::hook(c_lua_interface* self, const char* filename, const char* path,
	const char* string_to_run, bool run, bool print_errors, bool dont_push_errors, bool no_returns) {
	static c_lua_interface* last_self;
	static bool last_first;

	bool is_client = self == interfaces::lua_shared->get_lua_interface((int)e_interface_type::client);
	if (std::string(filename) != "RunString(Ex)")
		lua_futures::last_file_name = filename;

	auto is_first = false;
	if (self != last_self && interfaces::engine->is_drawing_loading_image())
		is_first = true;
	last_self = self;

	#if defined(_DEBUG)
		if (is_first) std::cout << filename << std::endl;
	#endif

	if (is_client) {
		std::string torun = string_to_run;
		auto menu_interface = interfaces::lua_shared->get_lua_interface((int)e_interface_type::menu);

		menu_interface->push_special((int)e_special::glob);
		menu_interface->get_field(-1, "hook");
		menu_interface->get_field(-1, "Call");
		menu_interface->push_string("RunOnClient");
		menu_interface->push_nil();
		menu_interface->push_string(filename);
		menu_interface->push_string(string_to_run);
		menu_interface->call(4, 1);
		if (!menu_interface->is_type(-1, (int)lua_object_type::NIL))
			torun = menu_interface->check_string();
		menu_interface->pop(3);

		return original(self, filename, path, torun.c_str(), run, print_errors, dont_push_errors, no_returns);
	}

	return original(self, filename, path, string_to_run, run, print_errors, dont_push_errors, no_returns);
}

void run_command_hook::hook(i_prediction* pred, c_base_entity* player, c_user_cmd* ucmd, i_move_helper* move_helper) {
	q_angle va;
	interfaces::engine->get_view_angles(va);
	original(pred, player, ucmd, move_helper);
	interfaces::engine->set_view_angles(va);
}

auto paint_traverse_hook::hook(i_panel* self, void* panel, bool force_repaint, bool allow_force) -> void {
	original(self, panel, force_repaint, allow_force);

	if (const std::string panel_name = interfaces::panel->get_name(panel); panel_name == "FocusOverlayPanel") {
		static auto numm = 0;
		numm++;
		if (globals::panic)
			return;
		directx_render::render_surface([&]() {
			esp::draw_esp();

			if (interfaces::engine->is_in_game() && settings::get_bool("aimbot_fov_draw")) {
				int w, h; interfaces::engine->get_screen_size(w, h);
				auto ratio = interfaces::engine->get_screen_aspect_ratio()/*(float)w / (float)h*/;
				const auto screen_fov = atanf((ratio) * (0.75f) * tan(math::deg2rad(globals::game_info::view_setup.fov * 0.5f)));
				const auto radius = tanf(math::deg2rad((float)settings::get_int("aimbot_fov"))) / tanf(screen_fov) * (w * 0.5f);
				directx_render::outlined_circle(ImVec2(w / 2, h / 2), radius, c_color(globals::colorfov[0] * 255.f, globals::colorfov[1] * 255.f, globals::colorfov[2] * 255.f, globals::colorfov[3] * 255.f));
			}

			if (settings::get_bool("aimbot_draw_target")) {
				auto target = aimbot::get_aimbot_target();
				if (target.is_valid()) {
					c_vector origin;
					if (game_utils::world_to_screen(target, origin)) {
						directx_render::line({ ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y }, (ImVec2)origin, c_color(globals::colortarger[0] * 255.f, globals::colortarger[1] * 255.f, globals::colortarger[2] * 255.f, globals::colortarger[3] * 255.f));
					}
				}
			}
		});
	}
}


void override_view_hook::hook(i_client_mode* self, c_view_setup& view) {
	static auto is_proof_inited = false;
	if (!is_proof_inited) {
		globals::game_info::proof_view_setup = view;
		is_proof_inited = true;
	}
	
	if (auto fov = settings::get_int("custom_fov"); fov > 0 && !globals::panic) {
		view.fov = static_cast<float>(fov);
	}

	if (settings::get_bool("norecoil") || settings::get_bool("third_person") && !globals::panic) {
		view.angles -= get_local_player()->get_view_punch_angles();
	}

	static bool should_reset_input_state;
	if (GetAsyncKeyState(settings::get_int("third_person_key")) & 1)
		globals::thirdtemp = !globals::thirdtemp;
	if (settings::get_bool("third_person") && globals::thirdtemp && !globals::panic) {
		c_vector view_vec; math::angle_to_vector(view.angles, view_vec);
		view_vec.invert();

		trace_t tr; ray_t ray;
		c_trace_filter f; f.pSkip = get_local_player();

		auto end_pos = view.origin + (view_vec * settings::get_int("third_person_distance"));
		ray.init(view.origin, end_pos);

		interfaces::engine_trace->trace_ray(ray, MASK_SHOT, &f, &tr);
		
		
			should_reset_input_state = true;
			interfaces::input->m_fCameraInThirdPerson = true;

			view.origin = tr.endpos;
		
	} else if (should_reset_input_state) {
		interfaces::input->m_fCameraInThirdPerson = false;
		should_reset_input_state = false;
	}
	
	globals::game_info::view_setup = view;
	
	return original(self, view);
}

float get_viewmodel_fov::hook() {
		
	if (auto fov = settings::get_int("custom_viewmodel_fov"); fov > 0 && !globals::panic) {
		return static_cast<float>(fov);
	}
	return original();
}

float get_aspect_ration_hook::hook(void* self) {
	
	if (auto rat = settings::get_int("custom_aspect_ratio"); rat > 0 && !globals::panic) {
		return (float)rat / 100.f;
	}
	return original(self);
}

c_vector savedPos = c_vector();

c_vector FreeCam(c_view_setup& view)
{
	c_vector    vPos = savedPos;
	q_angle    vView = view.angles;

	c_vector fwd, rt, up;
	math::angle_to_vectors(vView, fwd, rt, up);

	float speed = settings::get_int("freecam_speed");

	if (GetAsyncKeyState(VK_SHIFT))
		speed *= 2;

	for (int i = 0; i < 255; i++)
		if (GetAsyncKeyState(i))
			switch (i)
			{
			case VK_SPACE:
				vPos.z += 2.5f * speed;
				break;

			case VK_CONTROL:
				vPos.z -= 2.5f * speed;
				break;

			case 'W':
				vPos += fwd * speed;
				break;

			case 'S':
				vPos -= fwd * speed;
				break;

			case 'A':
				vPos -= rt * speed;
				break;

			case 'D':
				vPos += rt * speed;
				break;

			default:
				break;
			}

	return vPos;
}

bool render_view_hook::hook(void* view_render, c_view_setup& setup, int clear_flags, int what_to_draw) {
	if (GetAsyncKeyState(settings::get_int("freecam_key")) & 1 && settings::get_bool("freecam"))
		is_in_freecamera = !is_in_freecamera;
	if (settings::get_bool("freecam") && is_in_freecamera) {
		savedPos = FreeCam(setup);
		setup.fov_viewmodel = 180;
	}
	else {
		savedPos = setup.origin;
	}

	setup.origin = savedPos;
	return original(view_render, setup, clear_flags, what_to_draw);
}

void view_render_hook::hook(chl_client* self, vrect_t* rect) {
	original(self, rect);
}

void read_pixels_hook::hook(i_mat_render_context* self, int x, int y, int width, int height, unsigned char* data,
	image_format dstFormat) {
	static uintptr_t render_capture = 0;
	if (!render_capture) render_capture = (uintptr_t)memory_utils::pattern_scanner("client.dll", "40 55 41 57 48 8D 6C 24 ?");
	
	c_view_setup setup;
	if ((uintptr_t)_ReturnAddress() == render_capture) {
		interfaces::client->render_view(globals::game_info::proof_view_setup, view_clear_color | view_clear_depth,
																	 RENDERVIEW_DRAWHUD | RENDERVIEW_DRAWVIEWMODEL);
		
	}

	return original(self, x, y, width, height, data, dstFormat);
}

void frame_stage_notify_hook::hook(chl_client* self, int frame_stage) {

	return original(self, frame_stage);
}

LRESULT STDMETHODCALLTYPE wndproc_hook::hooked_wndproc(HWND window, UINT message_type, WPARAM w_param, LPARAM l_param)
{
	input_system::on_windpoc(message_type, w_param, l_param);
	input_system::process_binds();
	
	if (message_type == WM_CLOSE) {
		hack_utils::unload_hack();
		return true;
	}

	auto mk = settings::get_var<int>("menu_key");
	/*if (settings::binds["other::menu_key"] > 0)
		mk = settings::binds["other::menu_key"];*/

	if (message_type == WM_KEYDOWN) {
		if (w_param == mk&&!globals::panic)
			menu::toggle_menu();
		if (w_param == settings::get_var<int>("panic_key"))
			globals::panic = !globals::panic;
		if (globals::unload) {
			
			hooks_manager::shutdown();
		}
	}
	
	//ImGui_ImplWin32_WndProcHandler(window, message_type, w_param, l_param);
	if (ImGui_ImplWin32_WndProcHandler(window, message_type, w_param, l_param) && menu::menu_is_open())
	{
		return true;
	}

	
	return CallWindowProc(original_wndproc, window, message_type, w_param, l_param);
}
