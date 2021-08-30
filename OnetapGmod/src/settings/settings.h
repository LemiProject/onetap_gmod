#pragma once

#include "bind_system.h"
#include <string>

namespace settings {
	using namespace bind_system;
	class c_entity_list
	{
		std::vector<std::string> classes;
		std::mutex mutex;

	public:
		void push_back(const std::string& c);
		void remove(int idx);
		bool exist(const std::string& c);
		int find(const std::string& c);
		void exchange(const std::vector<std::string>& c);
		bool empty();
		void clear();

		size_t size();
		std::vector<std::string> data();
	};
	inline c_entity_list entitys_to_draw;
	using var_id_t = std::string;
	using var_id_t_non_copy = const var_id_t&;
	bool& get_bool(var_id_t_non_copy name);
	float& get_float(var_id_t_non_copy name);
	int& get_int(var_id_t_non_copy name);

	void set_bool(var_id_t_non_copy, bool val);
	void set_float(var_id_t_non_copy, float val);
	void set_int(var_id_t_non_copy, int val);

	bool is_bool(var_id_t_non_copy s);
	bool is_int(var_id_t_non_copy s);
	bool is_float(var_id_t_non_copy s);
}
