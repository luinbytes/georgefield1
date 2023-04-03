#pragma once
#include "Memory.h"
//#include "Source.h"
#include "Obfuscationmgr.h"
#include "Offsets.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <vector>
#include <format>
#include <string>
#include <algorithm>
#include <d3dx9.h>

struct vec3 {
public:
	float x, y, z;
	vec3() = default;
	explicit vec3(_In_reads_(3) const float* a) noexcept : x(a[0]), y(a[1]), z(a[2]) {};
	vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

	vec3 operator+(const vec3& v) {
		return vec3{ x + v.x, y + v.y, z + v.z };
	}
	vec3 operator+(const float& v) {
		return vec3{ x + v, y + v, z + v };
	}
	vec3 operator-(const vec3& v) {
		return vec3{ x - v.x, y - v.y, z - v.z };
	}
	vec3 operator-(const float& v) {
		return vec3{ x - v, y - v, z - v };
	}
	vec3 operator*(const vec3& v) {
		return vec3{ x * v.x, y * v.y, z * v.z };
	}
	vec3 operator*(const float& v) {
		return vec3{ x * v, y * v, z * v };
	}
	vec3 operator/(const vec3& v) {
		return vec3{ x / v.x, y / v.y, z / v.z };
	}
	vec3 operator/(const float& v) {
		return vec3{ x / v, y / v, z / v };
	}
	vec3& operator-=(const vec3& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	float length() const {
		return sqrt(x * x + y * y + z * z);
	}

	float dot(const vec3& other) const {
		return (x * other.x + y * other.y + z * other.z);
	}
	vec3 crossproduct(vec3 v2) const {
		return { (y * v2.z) - (z * v2.y), -((x * v2.z) - (z * v2.x)), (x * v2.y) - (y * v2.x) };
	}
};

struct vec4 {
	float x, y, z, w;
	vec4() = default;
	vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
};

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
	//bool world_to_screen(const Memory& mem, const vec3& WorldPos, vec3& ScreenPos);
	void log(std::string tekst, log_ yes = log_::info);
	void handle_menu();
	inline bool unload;
	inline HANDLE console;
	inline float smooth = 4.0f;
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