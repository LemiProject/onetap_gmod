#pragma once

#include "color.h"
#include "../../math/math.h"

class i_debug_overlay
{
public:
	/*0*/	virtual void* add_entity_text_overlay(int, int, float, int, int, int, int, char const*, ...) = 0;
	/*1*/	virtual void* add_box_overlay(c_vector const&, c_vector const&, c_vector const&, q_angle const&, int, int, int, int, float) = 0;
	/*2*/	virtual void* add_triangle_overlay(c_vector const&, c_vector const&, c_vector const&, int, int, int, int, bool, float) = 0;
	/*3*/	virtual void* add_line_overlay(c_vector const&, c_vector const&, int, int, int, bool, float) = 0;
	/*4*/	virtual void* add_text_overlay(c_vector const&, float, char const*, ...) = 0;
	/*5*/	virtual void* add_text_overlay(c_vector const&, int, float, char const*, ...) = 0;
	/*6*/	virtual void* add_screen_text_overlay(float, float, float, int, int, int, int, char const*) = 0;
	/*7*/	virtual void* add_swept_box_overlay(c_vector const&, c_vector const&, c_vector const&, c_vector const&, q_angle const&, int, int, int, int, float) = 0;
	/*8*/	virtual void* add_grid_overlay(c_vector const&) = 0;
	/*9*/	virtual bool screen_position(c_vector const& in, c_vector& out) = 0;
	/*10*/	virtual bool screen_position(float, float, c_vector&) = 0;
	/*11*/	virtual void* get_first(void) = 0;
	/*12*/	virtual void* get_next(void*) = 0;
	/*13*/	virtual void* clear_dead_overlays(void) = 0;
	/*14*/	virtual void* clear_all_overlays(void) = 0;
	/*15*/	virtual void* add_text_overlay_rgb(c_vector const&, int, float, float, float, float, float, char const*, ...) = 0;
	/*16*/	virtual void* add_text_overlay_rgb(c_vector const&, int, float, int, int, int, int, char const*, ...) = 0;
	/*17*/	virtual void* add_line_overlay_alpha(c_vector const&, c_vector const&, int, int, int, int, bool, float) = 0;
	/*18*/	virtual void* add_box_overlay2(c_vector const&, c_vector const&, c_vector const&, q_angle const&, c_color const&, c_color const&, float) = 0;
};