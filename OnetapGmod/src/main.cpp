#include <iostream>
#include <Windows.h>

#include <thread>

#include "globals.h"
#include "interfaces.h"
#include "hooks/hooks_manager.h"
#include "utils/input_system.h"

typedef void(__cdecl* warning_fn)(char const*, ...);

void entry_point() {
	interfaces::init_interfaces();
	hooks_manager::init();

	//input_system::add_bind(VK_MENU, "test_key", &globals::draw_box);
	
#if defined(_DEBUG) or defined(WITTCHEN_BETA)
	AllocConsole();
	freopen("CONOUT$", "wb", stdout);
	freopen("CONOUT$", "wb", stderr);
	freopen("CONIN$", "rb", stdin);
#endif
}

BOOL APIENTRY DllMain(HINSTANCE dll_instance, DWORD reason, LPVOID reversed)
{
	DisableThreadLibraryCalls(dll_instance);
	if (reason == DLL_PROCESS_ATTACH) {
		globals::dllInstance = dll_instance;
		std::thread(entry_point).detach();
	}
	return TRUE;
}