#pragma once

//TODO: IMPL THIS
class i_client_mode {
public:
	/*0*/	virtual void* destr() = 0;
	/*1*/	virtual void* InitViewport(void) = 0;
	/*2*/	virtual void* init(void) = 0;
	/*3*/	virtual void* v_gui_shutdown(void) = 0;
	/*4*/	virtual void* shutdown(void) = 0;
	/*5*/	virtual void* enable(void) = 0;
	/*6*/	virtual void* disable(void) = 0;
	/*7*/	virtual void* layout(void) = 0;
	/*8*/	virtual void* get_viewport(void) = 0;
	/*9*/	virtual void* get_viewport_animation_controller(void) = 0;
	/*10*/	virtual void* process_input(bool) = 0;
	/*11*/	virtual bool should_draw_detail_objects(void) = 0;
	/*12*/	virtual bool should_draw_entity(/*C_BaseEntity**/) = 0;
	/*13*/	virtual bool should_draw_local_player(/*C_BasePlayer**/) = 0;
	/*14*/	virtual bool should_draw_particles(void) = 0;
	/*15*/	virtual bool should_draw_fog(void) = 0;
	/*16*/	virtual void* override_view(void*) = 0;
	/*17*/	virtual void* key_input(int/*, ButtonCode_t*/, char const*) = 0;
	/*18*/	virtual void* start_message_mode(int) = 0;
	/*19*/	virtual void* get_message_panel(void) = 0;
	/*20*/	virtual void* override_mouse_input(float*, float*) = 0;
	/*21*/	virtual void* create_move(float /*CUserCmd**/) = 0;
	/*22*/	virtual void* level_init(char const*) = 0;
	/*23*/	virtual void* level_shutdown(void) = 0;
	/*24*/	virtual bool should_draw_view_model(void) = 0;
	/*25*/	virtual bool should_draw_crosshair(void) = 0;
	/*26*/	virtual void* adjust_engine_viewport(int&, int&, int&, int&) = 0;
	/*27*/	virtual void* pre_render(void*) = 0;
	/*28*/	virtual void* post_render(void) = 0;
	/*29*/	virtual void* post_render_v_gui(void) = 0;
	/*30*/	virtual void* activate_in_game_v_gui_context(void*) = 0;
	/*31*/	virtual void* deactivate_in_game_v_gui_context(void) = 0;
	/*32*/	virtual void* get_view_model_fov(void) = 0;
	/*33*/	virtual void* can_record_demo(char*, int)const = 0;
	/*34*/	virtual void* compute_vgui_res_conditions(void*) = 0;
	/*35*/	virtual const char* get_server_name(void) = 0;
	/*36*/	virtual const char* set_server_name(wchar_t*) = 0;
	/*37*/	virtual void* get_map_name(void) = 0;
	/*38*/	virtual void* set_map_name(wchar_t*) = 0;
	/*39*/	virtual void* do_post_screen_space_effects(void const*) = 0;
	/*40*/	virtual void* display_replay_message(char const*, float, bool, char const*, bool) = 0;
	/*41*/	virtual void* update(void) = 0;
	/*42*/	virtual bool should_blackout_around_hud(void) = 0;
	/*43*/	virtual bool should_override_headtrack_control(void) = 0;
	/*44*/	virtual bool is_info_panel_allowed(void) = 0;
	/*45*/	virtual void* info_panel_displayed(void) = 0;
	/*46*/	virtual void* reload_scheme(void) = 0;
	/*47*/	virtual void* hud_element_key_input(int /*ButtonCode_t*/, char const*) = 0;
	/*48*/	virtual void* handle_spectator_key_input(int /*ButtonCode_t*/, char const*) = 0;
	/*49*/	virtual void* get_hud_chat_panel(void) = 0;
	/*50*/	virtual void* update_replay_messages(void) = 0;
};