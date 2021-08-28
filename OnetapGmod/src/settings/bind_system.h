#pragma once

#include <cstdint>
#include <unordered_map>
#include <mutex>

#include <color.h>

namespace bind_system {
	enum class bind_type : int {
		none,
		pressed,
		toggle,
		active
	};

	constexpr const char* bind_type_string[4] = {"none", "pressed", "toggle", "active"};
	
	template <typename t>
	struct bind_t {
		int type = static_cast<int>(bind_type::none);
		uint32_t key = 0;
		t* value = nullptr;
		t bind_value = 0;
		t old_value = 0;
	};

	using float_bind = bind_t<float>;
	using int_bind = bind_t<int>;
	using uint_bind = bind_t<uint32_t>;
	using bool_bind = bind_t<bool>;
	using flags_bind_t = bind_t<int>;
	using color_bind_t = bind_t<c_color>;
	
	inline std::unordered_map<int64_t, std::vector<bool_bind>> bool_binds;
	inline std::unordered_map<int64_t, std::vector<int_bind>> int_binds;
	inline std::unordered_map<int64_t, std::vector<float_bind>> float_binds;
	inline std::unordered_map<int64_t, std::vector<color_bind_t>> color_binds;
	
	inline int64_t generate_bind_id(unsigned int itemid) {
		std::unordered_map<unsigned int, int64_t> ids;
		static std::mutex m;
		std::unique_lock l(m);
		if (ids.find(itemid) != ids.end())
			ids[itemid]++;
		else
			ids[itemid] = itemid;
		return ids[itemid];
	}
}
