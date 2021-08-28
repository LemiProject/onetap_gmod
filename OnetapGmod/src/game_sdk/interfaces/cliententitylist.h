#pragma once
#include <cstdint>

class c_client_entity_list
{
public:
		virtual void* constr(void*) = 0;
		virtual void* destr1() = 0;
		virtual void* destr2() = 0;
		virtual void* get_client_entity(int entnum) = 0;
		virtual void* get_client_entity_from_handle(uintptr_t hEnt) = 0;
		virtual int number_of_entities(bool b_include_non_networkable = false) = 0;
		virtual int get_highest_entity_index(void) = 0;
		virtual void set_max_entities(int maxents) = 0;
		virtual int get_max_entities(void) = 0;
};
