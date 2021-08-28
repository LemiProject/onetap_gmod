#pragma once
#include <string>

namespace lua_futures {
	void add_code_to_run(const std::string& str);

	void run_all_code();

	inline std::string last_file_name;

	extern std::string bypass;
}
