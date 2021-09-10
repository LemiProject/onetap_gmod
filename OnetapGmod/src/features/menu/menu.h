#pragma once
#include <d3d9.h>

namespace menu {
	void initialize_menu();
	
	void draw_menu();
	bool menu_is_open();
	void set_open_state(bool state);
	void toggle_menu();

}
