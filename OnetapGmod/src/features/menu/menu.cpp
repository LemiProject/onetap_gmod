#include "menu.h"

#include <imgui/imgui.h>
#include "../../interfaces.h"
#include "windows/main_window.h"
#include "windows/background_window.h"
#include "windows/glua_loader_window.h"
#include "../../settings/settings.h"
#include "../../globals.h"

bool is_menu_open;

constexpr auto animation_speed = 4.f;

_forceinline auto animation_add() {
	return ImGui::GetIO().DeltaTime * animation_speed;
}

void menu::initialize_menu() {
	
}

void menu::draw_menu() {
	if (globals::panic) {
		ImGui::GetIO().MouseDrawCursor = false;
		return;
	}
	if (menu_is_open())
	{
		if (!interfaces::surface->is_cursor_visible())
			ImGui::GetIO().MouseDrawCursor = true;

	
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);

		//background_window::draw_background_window();
		
		main_window::draw_main_window();
		//glua_loader_window::draw_glua_loader_window(globals::glua);
		ImGui::PopStyleVar();
	}
	else
	{
		ImGui::GetIO().MouseDrawCursor = false;
	}
}

bool menu::menu_is_open() {
	return is_menu_open;
}

void menu::set_open_state(bool state) {
	is_menu_open = state;
}

void menu::toggle_menu() {
	is_menu_open = !is_menu_open;
}

