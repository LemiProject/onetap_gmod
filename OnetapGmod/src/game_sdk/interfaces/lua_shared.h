#pragma once
#include "../../math/math.h"
#include <color.h>
#include <vector>

class i_lua_callback
{
public:

	/*0*/	virtual void* create_lua_object() = 0;
	/*1*/	virtual void destroy_lua_object(void* pObject) = 0;

	/*2*/	virtual void error_print(const char* strError) = 0;
	/*3*/	virtual void msg(const char* strMsg) = 0;

	/*4*/	virtual bool can_run_script(const char* strFilename, unsigned long CRC) = 0;
	/*5*/	virtual void on_run_script(const char* strFilename, bool bRun, const char* strScriptContents) = 0;
};
enum class lua_object_type
{

	INVALID = -1,
	NIL,
	BOOL,
	LIGHTUSERDATA,
	NUMBER,
	STRING,
	TABLE,
	FUNCTION,
	USERDATA,
	THREAD,

	ENTITY,
	VECTOR,
	ANGLE,
	PHYSOBJ,
	SAVE,
	RESTORE,
	DAMAGEINFO,
	EFFECTDATA,
	MOVEDATA,
	RECIPIENTFILTER,
	USERCMD,
	SCRIPTEDVEHICLE,

	MATERIAL,
	PANEL,
	PARTICLE,
	PARTICLEEMITTER,
	TEXTURE,
	USERMSG,

	CONVAR,
	IMESH,
	MATRIX,
	SOUND,
	PIXELVISHANDLE,
	DLIGHT,
	VIDEO,
	FILE,

	COUNT
};

enum class e_interface_type
{
	client,
	server,
	menu
};

enum class e_special
{
	glob = 0,
	env,
	reg
};

using number_t = double;
struct lua_State;
typedef int (*CFunc)(lua_State* L);

class i_lua_object;

class c_lua_interface
{
public:
	virtual int			top(void) = 0;
	virtual void		push(int iStackPos) = 0;
	virtual void		pop(int iAmt = 1) = 0;
	virtual void		get_table(int iStackPos) = 0;
	virtual void		get_field(int iStackPos, const char* strName) = 0;
	virtual void		set_field(int iStackPos, const char* strName) = 0;
	virtual void		create_table() = 0;
	virtual void		set_table(int i) = 0;
	virtual void		set_meta_table(int i) = 0;
	virtual bool		get_meta_table(int i) = 0;
	virtual void		call(int iArgs, int iResults) = 0;
	virtual int			p_call(int iArgs, int iResults, int iErrorFunc) = 0;
	virtual int			equal(int iA, int iB) = 0;
	virtual int			raw_equal(int iA, int iB) = 0;
	virtual void		insert(int iStackPos) = 0;
	virtual void		remove(int iStackPos) = 0;
	virtual int			next(int iStackPos) = 0;
	virtual void* new_userdata(unsigned int iSize) = 0;
	virtual void		throw_error(const char* strError) = 0;
	virtual void		check_type(int iStackPos, int iType) = 0;
	virtual void		arg_error(int iArgNum, const char* strMessage) = 0;
	virtual void		raw_get(int iStackPos) = 0;
	virtual void		raw_set(int iStackPos) = 0;
	virtual const char* get_string(int iStackPos = -1, unsigned int* iOutLen = NULL) = 0;
	virtual double			get_number(int iStackPos = -1) = 0;
	virtual bool			get_bool(int iStackPos = -1) = 0;
	virtual void* get_c_function(int iStackPos = -1) = 0;
	virtual void* get_userdata(int iStackPos = -1) = 0;
	virtual void		push_nil() = 0;
	virtual void		push_string(const char* val, unsigned int iLen = 0) = 0;
	virtual void		push_number(double val) = 0;
	virtual void		push_bool(bool val) = 0;
	virtual void		push_c_function(void* val) = 0;
	virtual void		push_c_closure() = 0;
	virtual void		push_userdata(void*) = 0;
	virtual int			reference_create() = 0;
	virtual void		reference_free(int i) = 0;
	virtual void		reference_push(int i) = 0;
	virtual void		push_special(int iType) = 0;
	virtual bool			is_type(int iStackPos, int iType) = 0;
	virtual int				get_type(int iStackPos) = 0;
	virtual const char* get_type_name(int iType) = 0;
	virtual void			create_meta_table_type(const char* strName, int iType) = 0;
	virtual const char* check_string(int iStackPos = -1) = 0;
	virtual double	check_number(int iStackPos = -1) = 0;
	virtual void obj_len(int) = 0;
	virtual void get_angle(int) = 0;
	virtual void* get_vector(int) = 0;
	virtual void push_angle(void*) = 0;
	virtual void push_vector(void*) = 0;
	virtual void set_state(lua_State* l) = 0;
	virtual void create_meta_table(char const*) = 0;
	virtual void push_meta_table(int) = 0;
	virtual void push_user_type(void*, int) = 0;
	virtual void set_user_type(int, void*) = 0;
	virtual void init(void*, bool) = 0;
	virtual void shutdown(void) = 0;
	virtual void cycle(void) = 0;
	virtual void global(void) = 0;
	virtual i_lua_object* get_object(int) = 0;
	virtual void push_lua_object(void*) = 0;
	virtual void push_lua_function() = 0;
	virtual void lua_error(char const*, int) = 0;
	virtual void type_error(char const*, int) = 0;
	virtual void call_internal(int, int) = 0;
	virtual void call_internal_no_returns(int) = 0;
	virtual bool call_internal_get_bool(int) = 0;
	virtual void call_internal_get_string(int) = 0;
	virtual void call_internal_get(int, void*) = 0;
	virtual void new_global_table(char const*) = 0;
	virtual void new_temporary_object(void) = 0;
	virtual void is_user_data(int) = 0;
	virtual void get_meta_table_object(char const*, int) = 0;
	virtual void get_meta_table_object(int) = 0;
	virtual void get_return(int) = 0;
	virtual void is_server(void) = 0;
	virtual void is_client(void) = 0;
	virtual void is_dedicated_server(void) = 0;
	virtual void destroy_object(void*) = 0;
	virtual void create_object(void) = 0;
	virtual void set_member(void*, void*, void*) = 0;
	virtual void get_new_table(void) = 0;
	virtual void set_member(void*, float) = 0;
	virtual void set_member(void*, float, void*) = 0;
	virtual void set_member(void*, char const*) = 0;
	virtual void set_member(void*, char const*, void*) = 0;
	virtual void set_is_server(bool) = 0;
	virtual void push_long(long) = 0;
	virtual void get_flags(int) = 0;
	virtual void find_on_objects_meta_table(int, int) = 0;
	virtual void find_object_on_table(int, int) = 0;
	virtual void set_member_fast(void*, int, int) = 0;
	virtual void run_string(char const* filename, char const* path, char const* stringtoun, bool run = true, bool showerrors = true) = 0;
};


enum lua_find_result
{
	i_have_no_idea
};

class c_lua_shared
{
public:
	/*0*/	virtual void* destr1() = 0;
	/*2*/	virtual void* init(void* (*)(char const*, int*), bool, void*, void*) = 0;
	/*3*/	virtual void* shutdown(void) = 0;
	/*4*/	virtual void* dump_stats(void) = 0;
	/*5*/	virtual c_lua_interface* create_lua_interface(unsigned char, bool) = 0;
	/*6*/	virtual void* close_lua_interface(void*) = 0;
	/*7*/	virtual c_lua_interface* get_lua_interface(unsigned char) = 0;
	/*8*/	virtual void load_file(std::string const&, std::string const&, bool, bool) = 0;
	/*9*/	virtual void* get_cache(std::string const&) = 0;
	/*10*/	virtual void* mount_lua(char const*) = 0;
	/*11*/	virtual void* mount_lua_add(char const*, char const*) = 0;
	/*12*/	virtual void* un_mount_lua(char const*) = 0;
	/*13*/	virtual void* set_file_contents(char const*, char const*) = 0;
	/*14*/	virtual void* set_lua_find_hook(void*) = 0;
	/*15*/	virtual void* find_scripts(std::string const&, std::string const&, std::vector<lua_find_result>&) = 0;
	/*16*/	virtual void* get_stack_traces(void) = 0;
	/*17*/	virtual void* invalidate_cache(std::string const&) = 0;
	/*18*/	virtual void* empty_cache(void) = 0;
};

class c_lua_auto_pop
{
	c_lua_interface* i;
	int top;
public:
	c_lua_auto_pop(c_lua_interface* intr);
	~c_lua_auto_pop();
};


inline c_lua_auto_pop::c_lua_auto_pop(c_lua_interface* intr)
{
	top = intr->top();
	i = intr;
}

inline c_lua_auto_pop::~c_lua_auto_pop()
{
	i->pop(i->top() - top);
}

namespace lua_tools {
	static __forceinline void compile_string(c_lua_interface* in, const std::string& str, int output_num = 0, bool handle_errors = true) {
		in->push_special((int)e_special::glob); //1
		in->get_field(-1, "CompileString"); //1
		in->push_string(str.c_str()); //2
		in->push_string((std::string("CompileString") + str[0] + str.back()).c_str()); //3
		in->push_bool(handle_errors); //4
		in->call(3, 1); //2
		in->push(2); //push function from return
		in->call(0, output_num);
	}
}
