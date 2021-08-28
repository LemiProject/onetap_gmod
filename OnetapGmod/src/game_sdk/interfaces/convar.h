#pragma once

#define FCVAR_ARCHIVE 128
#define FCVAR_ARCHIVE_XBOX 16777216
#define FCVAR_CHEAT 16384
#define FCVAR_CLIENTCMD_CAN_EXECUTE 1073741824
#define FCVAR_CLIENTDLL 8
#define FCVAR_DEMO 65536
#define FCVAR_DONTRECORD 131072
#define FCVAR_GAMEDLL 4
#define FCVAR_LUA_CLIENT 262144
#define FCVAR_LUA_SERVER 524288
#define FCVAR_NEVER_AS_STRING 4096
#define FCVAR_NONE 0
#define FCVAR_NOTIFY 256
#define FCVAR_NOT_CONNECTED 4194304
#define FCVAR_PRINTABLEONLY 1024
#define FCVAR_PROTECTED 32
#define FCVAR_REPLICATED 8192
#define FCVAR_SERVER_CANNOT_QUERY 536870912
#define FCVAR_SERVER_CAN_EXECUTE 268435456
#define FCVAR_SPONLY 64
#define FCVAR_UNLOGGED 2048
#define FCVAR_UNREGISTERED 1
#define FCVAR_USERINFO 512
#include <cstdint>
#include <color.h>

inline void dummy_callback(void* convar, const char* pOldValString) { return; }

class con_var
{
public:
	con_var* pNextConvar; //0x0008
	uint32_t bRegistered; //0x0010
	char pad_0014[4]; //0x0014
	char* pszName; //0x0018
	char* pszHelpString; //0x0020
	uint32_t nflags; //0x0028
	char pad_002C[4]; //0x002C
	void* s_pConCommandBases; //0x0030
	void* pParent; //0x0038-
	char* pszDefaultValue; //0x0040
	char* pszValueStr; //0x0048
	uint32_t strLength; //0x0050
	float fVal; //0x0054
	int32_t intValue; //0x0058
	uint32_t bHasMin; //0x005C
	float fMinVal; //0x0060
	uint32_t bHasMax; //0x0064
	float fMaxVal; //0x0068
	char pad_006C[4]; //0x006C
	PVOID CALLBACKPTR; //0x0070


	//Don't forget the constructor.
	/*0*/	virtual void* destr() = 0;
	/*2*/	virtual bool is_command(void)const = 0;
	/*3*/	virtual bool is_flag_set(int)const = 0;
	/*4*/	virtual void add_flags(int) = 0;
	/*4*/	virtual void remove_flags(int) = 0;

	/*4*/	virtual uint64_t get_flags() = 0;

	/*7*/	virtual const char* get_name(void)const = 0;
	/*8*/	virtual const char* get_help_text(void)const = 0;
	/*9*/	virtual bool is_registered(void)const = 0;
	/*10*/	virtual void* get_dll_identifier(void)const = 0;
	/*11*/	virtual void create_base(char const*, char const*, int) = 0;
	/*11*/	virtual void init(void) = 0;

	/*11*/	virtual void unk1(void) = 0;
	/*11*/	virtual void unk2(void) = 0;
	///*12*/	virtual void SetValue(char const*) = 0;
	///*13*/	virtual void SetValue(float) = 0;
	///*14*/	virtual void SetValue(int) = 0;
	/*15*/	virtual void internal_set_value(char const*) = 0; // This doesn't works for me, for some reasons
	/*16*/	virtual void internal_set_value(float) = 0;
	/*17*/	virtual void InternalSetValue(int) = 0;
	/*18*/	virtual void set_value(int) = 0;
	/*19*/	virtual void* clamp_value(float&) = 0;
	/*20*/	virtual void* change_string_value(char const*, float) = 0;
	/*21*/	virtual void* create(char const*, char const*, int, char const*, bool, float, bool, float, void (*)(void*, char const*, float)) = 0;

	void disable_callback() {
		if (this->CALLBACKPTR)
			*(void**)(this->CALLBACKPTR) = dummy_callback;
	}
};

typedef void* c_var_dll_identifier_t;
typedef void* con_command_base;
typedef void* con_command;
typedef void* fn_change_callback_t;

class c_cvar
{
public:
	virtual void connect(void* (*)(char const*, int*)) = 0;
	virtual void disconnect(void) = 0;
	virtual void* query_interface(char const*) = 0;
	virtual void* init(void) = 0;
	virtual void shutdown(void) = 0;

	virtual void* nothing1(void) = 0;
	virtual void* nothing2(void) = 0;
	virtual void* nothing3(void) = 0;
	virtual void* nothing4(void) = 0;

	virtual c_var_dll_identifier_t        allocate_dll_identifier() = 0; // 9
	virtual void                       register_con_command(con_command_base* pCommandBase) = 0; //10
	virtual void                       unregister_con_command(con_command_base* pCommandBase) = 0;
	virtual void                       unregister_con_commands(c_var_dll_identifier_t id) = 0;
	virtual const char* get_command_line_value(const char* pVariableName) = 0;
	virtual con_command_base* find_command_base(const char* name) = 0;
	virtual const con_command_base* find_command_base(const char* name) const = 0;
	virtual con_var* find(const char* var_name) = 0; //16
	virtual const con_var* find(const char* var_name) const = 0;
	virtual con_command* find_command(const char* name) = 0;
	virtual const con_command* find_command(const char* name) const = 0;
	virtual void                       install_global_change_callback(fn_change_callback_t callback) = 0;
	virtual void                       remove_global_change_callback(fn_change_callback_t callback) = 0;
	virtual void                       call_callbacks(con_var* var, const char* pOldString, float flOldValue) = 0;
	virtual void                       install_console_display_func(void* pDisplayFunc) = 0;
	virtual void                       remove_console_display_func(void* pDisplayFunc) = 0;
	virtual void                       console_color_printf(const c_color& clr, const char* pFormat, ...) const = 0;
	virtual void                       console_printf(const char* pFormat, ...) const = 0;
	virtual void                       console_d_printf(const char* pFormat, ...) const = 0;
	virtual void                       revert_flagged_con_vars(int nFlag) = 0;
};