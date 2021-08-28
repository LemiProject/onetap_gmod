#pragma once

class i_surface {
public:
	void lock_cursor()
	{
		using fn = void(__stdcall*)(void*);
		return (*(fn**)this)[66](this);
	}
	
	void unlock_cursor()
	{
		using fn = void(__stdcall*)(void*);
		return (*(fn**)this)[65](this);
	}

	bool is_cursor_visible() {
		using fn = bool(__fastcall*)(void*);
		return (*(fn**)this)[57](this);
	}
	
};