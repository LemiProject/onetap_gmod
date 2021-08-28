#pragma once

#include "game_sdk/interfaces/engine.h"
#include "game_sdk/interfaces/client.h"
#include "game_sdk/interfaces/cliententitylist.h"
#include "game_sdk/interfaces/client_mode.h"
#include "game_sdk/interfaces/surface.h"
#include "game_sdk/interfaces/lua_shared.h"
#include "game_sdk/interfaces/i_panel.h"
#include "game_sdk/interfaces/debugoverlay.h"
#include "game_sdk/interfaces/imaterialsystem.h"
#include "game_sdk/interfaces/convar.h"
#include "game_sdk/interfaces/random_stream.h"
#include "game_sdk/interfaces/iprediction.h"
#include "game_sdk/interfaces/ienginetrace.h"
#include "game_sdk/interfaces/globalvars.h"
#include "game_sdk/interfaces/gamemovement.h"
#include "game_sdk/interfaces/movehelper.h"
#include "game_sdk/interfaces/cinput.h"

namespace interfaces {
	inline c_engine_client* engine;
	inline c_client_entity_list* entity_list;
	inline chl_client* client;
	inline i_client_mode* client_mode;
	inline i_surface* surface;
	inline c_lua_shared* lua_shared;
	inline i_panel* panel;
	inline i_debug_overlay* debug_overlay;
	inline i_mat_render_context* mat_render_context;
	inline i_material_system* material_system;
	inline c_cvar* cvar;
	inline c_uniform_random_stream* random_stream;
	inline i_prediction* prediction;
	inline i_engine_trace* engine_trace;
	inline c_global_vars* global_vars;
	inline i_game_movement* game_movement;
	inline i_move_helper* move_helper;
	inline CInput* input;
	
	void init_interfaces();
}
