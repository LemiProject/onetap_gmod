#pragma once

#include <string>
#include <any>
#include <filesystem>

#include "bind_system.h"

#define IF_NOT_EXIST_CREATE_VAR_ false
//#define IF_NOT_EXIST_CREATE_VAR_ TRUE

namespace settings {
	using namespace bind_system;
	
	using var_id_t = std::string;
	using var_id_t_non_copy = const var_id_t&;

	namespace internal {
		//ONLY FOR INTERNAL USING

		using settings_storage_type = std::unordered_map<std::string, std::any>;

		settings_storage_type& get_settings_storage();
	}

	inline bool var_exist(var_id_t_non_copy name) {
		auto& storage = internal::get_settings_storage(); 
		return storage.find(name) != storage.end();
	}

	template <typename t>
	inline t& get_var(var_id_t_non_copy name) {
		if (!var_exist(name))
			if constexpr (IF_NOT_EXIST_CREATE_VAR_)
				internal::get_settings_storage()[name] = 0;
			else {
				throw std::exception(("Cannot find variable " + name).c_str());
			}
		try {
			return *std::any_cast<t>(&internal::get_settings_storage()[name]);
		} catch (...) {
			throw std::exception(("Value type error. Var: " + name).c_str());
		}
	}
	
	template <typename t>
	inline void set_var(var_id_t_non_copy name, const t& data) {
		internal::get_settings_storage()[name] = data;
	}

	template <typename t>
	inline bool is_type(var_id_t_non_copy name) {
		if (!var_exist(name)) throw std::exception(("Cannot find variable " + name).c_str());
		return internal::get_settings_storage()[name].type() == typeid(t);
	}

	bool& get_bool(var_id_t_non_copy name);
	float& get_float(var_id_t_non_copy name);
	int& get_int(var_id_t_non_copy name);

	void set_bool(var_id_t_non_copy, bool val);
	void set_float(var_id_t_non_copy, float val);
	void set_int(var_id_t_non_copy, int val);

	bool is_bool(var_id_t_non_copy s);
	bool is_int(var_id_t_non_copy s);
	bool is_float(var_id_t_non_copy s);

	void save_to_file(const std::filesystem::path& path);
	void load_from_file(const std::filesystem::path& path);
}
