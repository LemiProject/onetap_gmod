#include "interfaces.h"

#include "utils/memoryutils.h"

void interfaces::init_interfaces() {
	engine = memory_utils::capture_interface<c_engine_client>("engine.dll", "VEngineClient015");
	client = memory_utils::capture_interface<chl_client>("client.dll", "VClient017");
	entity_list = memory_utils::capture_interface<c_client_entity_list>("client.dll", "VClientEntityList003");
	surface = memory_utils::capture_interface<i_surface>("vguimatsurface.dll", "VGUI_Surface030");
	lua_shared = memory_utils::capture_interface<c_lua_shared>("lua_shared.dll", "LUASHARED003");
	panel = memory_utils::capture_interface<i_panel>("vgui2.dll", "VGUI_Panel009");
	debug_overlay = memory_utils::capture_interface<i_debug_overlay>("engine.dll", "VDebugOverlay003");
	material_system = memory_utils::capture_interface<i_material_system>("materialsystem.dll", "VMaterialSystem080");
	cvar = memory_utils::capture_interface<c_cvar>("vstdlib.dll", "VEngineCvar007");
	prediction = memory_utils::capture_interface<i_prediction>("client.dll", "VClientPrediction001");
	engine_trace = memory_utils::capture_interface<i_engine_trace>("engine.dll", "EngineTraceClient003");
	game_movement = memory_utils::capture_interface<i_game_movement>("client.dll", "GameMovement001");

	input = memory_utils::get_vmt<CInput>((uintptr_t)client, 20, 0x0);
	
	move_helper = memory_utils::get_vmt_from_instruction<i_move_helper>((uintptr_t)memory_utils::pattern_scanner("client.dll", "48 8B 0D ? ? ? ? 48 8B 53 10"));
	
	//client mode pattern: CHLClient__HudProcessInput + offset to mov [reg], g_ClientMode. Sizeof mov instruction - 3. 
	client_mode = memory_utils::get_vmt<i_client_mode>((uintptr_t)client, 10, 0x0);

	mat_render_context = material_system->get_render_context();

	//global_vars = (c_global_vars*)memory_utils::relative_to_absolute((uintptr_t)(memory_utils::pattern_scanner("client.dll", "48 8B 05 ? ? ? ? 48 8B D7")), 0x3, 7);

	random_stream = memory_utils::get_vmt_from_instruction<c_uniform_random_stream>((uintptr_t)GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomSeed"), 0x2);
}
