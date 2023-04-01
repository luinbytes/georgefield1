#pragma once
#include <iostream>
#include <thread>
#include <windows.h>
#include <fstream>
#include <vector>
#include "memory.h"
#include <d3dx9.h>
#include <format>

enum log_ {
	info = 15,
	eror = 12,
	fatal = 79,
	gude = 10
};

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

namespace balls
{
	void init();
	void moveing(const Memory& mem);
	bool world_to_screen(const Memory& mem, const vec3& WorldPos, vec3& ScreenPos);
	void log(std::string tekst, log_ yes = log_::info);
	void handle_menu();
	inline HANDLE console;

	inline float smooth = 2.0f;
	inline float fov = 80.0f;
	inline int sensitivity = 10;
	inline int aimpriority = 0; //0 - Closest. 1 - Closest head. 2 - Closest body.
	inline std::string grogtext = "george         ";
	inline uintptr_t client;
	inline int width, height, cw, ch;
	inline D3DXMATRIXA16 view_x_projection;
	inline D3DXMATRIXA16 view_inverse;
	inline bool key_held(int key)
	{
		return (GetAsyncKeyState(key) & 0x8000);
	}

}

enum DudeBones
{
	//Torso
	m_HIPS = 4,
	m_PELVIS = 5,
	m_LOWERCHEST = 6,
	m_UPPERCHEST = 7,
	//Head
	m_NECK = 43,
	m_HEAD = 45,
	m_TOPHEAD = 46,
	m_FACE = 47,
	//Left Side
	m_LEFTHIPS = 189,
	m_LEFTSHOULDER = 8,
	m_LEFTARM = 9,
	m_LEFTHAND = 14,
	m_LEFTELBOW = 40,
	m_LEFTEYE = 63,
	m_LEFTUPPERLEG = 181,
	m_LEFTLEG = 183,
	m_LEFTFOOT = 184,
	m_LEFTKNEE = 188,
	m_LEFTHIPROLL = 189,
	m_LEFTTOE = 186,
	m_LEFTUPPERARM = 41,
	//Right Side
	m_RIGHTHIPS = 205,
	m_RIGHTEYE = 65,
	m_RIGHTSHOULDER = 109,
	m_RIGHTARM = 121,
	m_RIGHTHAND = 139,
	m_RIGHTELBOW = 140,
	m_RIGHTUPPERLEG = 195,
	m_RIGHTLEG = 198,
	m_RIGHTFOOT = 198,
	m_RIGHTKNEE = 197,
	m_RIGHTHIPROLL = 203,
	m_RIGHTTOE = 185,
	m_RIGHTUPPERARM = 141,
	//WEAPON 144-178 enum
	m_WEAPONBASE = 155,
	m_WEAPONSLIDE = 158,
	m_WEAPONMAG = 161
};