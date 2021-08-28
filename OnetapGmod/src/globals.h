#pragma once

#include <Windows.h>

#include "game_sdk/misc/viewsetup.h"

namespace globals {
	inline HINSTANCE dllInstance;

	namespace game_info {
		inline c_view_setup view_setup;
		inline c_view_setup proof_view_setup;
		inline int chocked_packets;
	}
}


enum class e_bones : int {
	none = (0 << 0),
	head = (1 << 0),
	body = (1 << 1),
	pelvis = (1 << 2),
	arm = (1 << 3),
	foot = (1 << 4),
	last = foot
};

inline const char* to_string(e_bones e) {
	switch (e) {
	case e_bones::none: return "";
	case e_bones::head: return "head";
	case e_bones::body: return "body";
	case e_bones::pelvis: return "pelvis";
	case e_bones::arm: return "arm";
	case e_bones::foot: return "foot";
	default: return "";
	}
}
