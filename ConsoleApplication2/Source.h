#pragma once
#include "Includes.h"


enum log_ {
	info = 15,
	eror = 12,
	fatal = 79,
	gude = 10
};

namespace b
{
	void init();
	void moveing(const Memory& mem);
	bool world_to_screen(const Memory& mem, const vec3& WorldPos, vec3& ScreenPos);
	void log(std::string tekst, log_ yes = log_::info);
	void handle_menu();
	inline bool unload;
	inline HANDLE console;

	inline float smooth = 2.2f;
	inline float fov = 80.0f;
	inline int sensitivity = 10;
	inline bool norecoil = false;
	inline bool nospread = false;
	inline int aimprority = 0;//0 closest all,1 closest head ,2 closest body
	inline std::string grogtext = "george         ";
	inline uintptr_t client;
	inline D3DXMATRIXA16 view_x_projection;
	inline D3DXMATRIXA16 view_inverse;
	inline bool key_held(int key)
	{
		return (GetAsyncKeyState(key) & 0x8000);
	}
}