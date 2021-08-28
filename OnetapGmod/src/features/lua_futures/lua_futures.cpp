#include "lua_futures.h"

#include <mutex>
#include <vector>

#include "../../interfaces.h"

std::mutex lua_run_mutex;
std::vector<std::string> lua_to_run;

void lua_futures::add_code_to_run(const std::string& str) {
	std::unique_lock l(lua_run_mutex);
	lua_to_run.push_back(str);
}

void lua_futures::run_all_code() {
	std::unique_lock l(lua_run_mutex);

	if (lua_to_run.empty())
		return;

	auto intr = interfaces::lua_shared->get_lua_interface(0);
	if (!intr)
		return;
	
	for (const auto& i : lua_to_run) {
		try {
			intr->run_string(last_file_name.c_str(), "", i.c_str(), true, true);
		}
		catch (...) {
		}
	}

	lua_to_run.clear();
}

std::string lua_futures::bypass = u8R"(
)";