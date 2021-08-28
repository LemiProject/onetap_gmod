#include "spreads.h"

#include "../../game_sdk/entities/c_base_weapon.h"

#include "../../utils/md5_check_sum.h"
#include "../../settings/settings.h"

void spreads::base_nospread(c_user_cmd& cmd) {
	auto wep = get_local_player()->get_active_weapon();
	if (!wep && get_local_player()->get_health()<1)
		return;
	
	auto spread_cone = wep->get_bullet_spread();

	if (((std::string)(wep->get_lua_script_name())).find("m9k") != std::string::npos) {
		auto in = interfaces::lua_shared->get_lua_interface((int)e_interface_type::client);
		c_lua_auto_pop p(in);
		auto s = wep->get_primary_value("Spread");
		spread_cone = {s};
	}
	
	if (spread_cone == q_angle(-1.f))
		return;

	const auto spread = ((spread_cone.x + spread_cone.y + spread_cone.z) / 3.f);

	float random[2];
	if (cmd.command_number == 0)
		return;
	auto seed = md5::md5_pseudo_random(cmd.command_number) & 0xFF;

	interfaces::random_stream->set_seed(seed);

	random[0] = interfaces::random_stream->random_float(-0.5f, 0.5f)
		+ interfaces::random_stream->random_float(-0.5f, 0.5f);

	random[1] = interfaces::random_stream->random_float(-0.5f, 0.5f)
		+ interfaces::random_stream->random_float(-0.5f, 0.5f);

	auto dir = (c_vector(1.0f, 0.0f, 0.0f) + (c_vector(0.0f, -1.0f, 0.0f) * spread * random[0]) + (c_vector(0.0f, 0.0f, 1.0f) * spread * random[1]));

	q_angle out = math::get_angle(q_angle(0.f, 0.f, 0.f), dir);
	if (!settings::get_bool("norecoil"))
		out += get_local_player()->get_view_punch_angles();
	else
		out -= get_local_player()->get_view_punch_angles();

	out = math::fix_angles(out);

	if (out.is_valid())
		cmd.viewangles -= out;
}

void spreads::swb_nospread(c_user_cmd& cmd) {
	auto wep = get_local_player()->get_active_weapon();
	if (!wep &&get_local_player()->get_health() < 1)
		return;

	c_vector spread_cone;
	{
		auto in = interfaces::lua_shared->get_lua_interface((int)e_interface_type::client);
		c_lua_auto_pop p(in);
		wep->push_entity();
		in->get_field(-1, "CurCone");
		auto s = (float)in->get_number();
		spread_cone = { s };
	}
	auto cone = spread_cone.x;

	math::lua::random_seed(cmd.command_number);

	c_vector rand = {(float)math::lua::rand(-cone, cone),
	(float)math::lua::rand(-cone, cone), 0.f };

	q_angle pa = get_local_player()->get_view_punch_angles();

	q_angle ang;
	if (settings::get_bool("norecoil"))
		ang = cmd.viewangles - (rand * 25.f);
	else
		ang = cmd.viewangles - (pa + (rand * 25.f));

	cmd.viewangles = ang;
}

void spreads::fas2_nospread(c_user_cmd& cmd) {
	/*auto wep = get_local_player()->get_active_weapon();
	if (!wep)
		return;
	
	auto lua = interfaces::lua_shared->get_lua_interface((int)e_interface_type::client);
	if (!lua)
		return;
	c_lua_auto_pop ap(lua);

	float cone;

	{
		c_lua_auto_pop ap2(lua);
		wep->push_entity();
		lua->get_field(-1, "CurCone");
		cone = lua->get_number();
	}

	if (cone == 0.f)
		return;

	//math::lua::random_seed(get_local_player()->get_tick_base() * interfaces::global_vars->interval_per_tick);
	float x = math::lua::rand(-cone, cone);
	float y = math::lua::rand(-cone, cone);

	auto ang = q_angle(-x, -y, 0) * 25.f;

	if (ang.is_valid()) cmd.viewangles += ang;*/
}
