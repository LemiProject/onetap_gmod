#pragma once

#include "../../game_sdk/entities/c_base_player.h"
#include "../../game_sdk/misc/usercmd.h"

namespace aimbot {
	class c_aimbot_trace_filter : public i_trace_filter {
	public:
		bool should_hit_entity(void* pEntityHandle, int contentsMask)
		{
			return pEntityHandle != lp && pEntityHandle != ent;
		}
		virtual trace_type_t get_trace_type() const
		{
			return trace_everything;
		}
		void* lp;
		void* ent;
	};
	
	c_vector get_aimbot_target();

	void run_aimbot(c_user_cmd& cmd);
	
	void norecoil(c_user_cmd& cmd);
	void nospread(c_user_cmd& cmd);

	bool start_prediction(c_user_cmd& cmd);
	void end_prediction();
}
