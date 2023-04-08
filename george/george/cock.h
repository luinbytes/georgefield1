#pragma once
#include <iostream>
#include <thread>
#include <windows.h>
#include <fstream>
#include <vector>
#include "memory.h"
#include < d3dx9.h >
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
	explicit vec3(_In_reads_(3) const float* a) noexcept : x(a[0]),y(a[1]),z(a[2]) {};
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

#define offset const long long
namespace b
{
	struct {
		offset game_context = 0x1437F7758;
		offset game_render = 0x1439e6d08;
		offset obf_mngr = 0x14351D058;
		offset dx = 0x142738080;	
		offset cur_wpn = 0x1423B2EC8;
		offset player_manager = 0x68;
		offset local_player = 0x540;
		offset player = 0x548;
		offset soldier = 0x1D48;
		offset team = 0x1C34;
		offset position = 0x0990;
		offset position_origin = 0x30;
		offset position_velocity = 0x50;
		offset healthclass = 0x01D0;
		offset health = 0x20;
		offset level = 0x0028;
		offset gameworld = 0x0130;
		offset ragdoll_component = 0x0580;
		offset valid_transform = 0xC8;
		offset pQuat = 0xB0;
	} offsets;

	void init();
	void wpn_modifier(const Memory& mem);
	void aimbob(const Memory& mem);
	bool world_to_screen(const Memory& mem, const vec3& WorldPos, vec3& ScreenPos);
	void log(std::string tekst, log_ yes = log_::info);
	void handle_menu();
	inline bool unload;
	inline HANDLE console;

	const auto process_name = L"bf1.exe";
	inline float smooth = 1.8f;
	inline float fov = 60.0f;
	inline int sensitivity = 10;
	inline bool nospreadnorekoil = false;
	inline int aimprority = 0;//0 closest all,1 closest head ,2 closest body
	const std::string aimtypes[3] = { "all","head","body" };
	inline std::string grogtext = "    george    ";
	inline uintptr_t client;
	inline int width, height, cw, ch;
	inline D3DXMATRIXA16 view_x_projection;
	inline D3DXMATRIXA16 view_inverse;
	inline uintptr_t cur_wpn;
	inline bool key_held(int key)
	{
		return (GetAsyncKeyState(key) & 0x8000);
	}
}

inline bool valid(uintptr_t ptr, std::string text) {
#ifndef NDEBUG
	if (ptr >= 0x10000 && ptr < 0x000F000000000000)
		return true;
	b::log(std::format("{} was invalid: {}", text, ptr));
	return false;
#else
	return (ptr >= 0x10000 && ptr < 0x000F000000000000);
#endif
}
#define is_valid(var) valid(var,#var)

enum BoneIds
{
	BONE_Head = 0x35,
	BONE_Neck = 0x33,
	BONE_SPINE2 = 0x7,
	BONE_SPINE1 = 0x6,
	BONE_Spine = 0x5,
	BONE_LeftShoulder = 0x8,
	BONE_RightShoulder = 0xA3,
	BONE_LeftElbowRoll = 0xE,
	BONE_RightElbowRoll = 0xA9,
	BONE_LeftHand = 0x10,
	BONE_RightHand = 0xAB,
	BONE_RightKneeRoll = 0x12D,
	BONE_LeftKneeRoll = 0x11F,
	BONE_LeftFoot = 0x115,
	BONE_RightFoot = 0x123
};

inline uint64_t EncryptedPlayerMgr_GetPlayer(const Memory& mem, uint64_t ptr, int id)
{
	uint64_t XorValue1 = mem.Read<uint64_t>(ptr + 0x20) ^ mem.Read<uint64_t>(ptr + 0x8);
	uint64_t XorValue2 = XorValue1 ^ mem.Read<uint64_t>(ptr + 0x10);
	if (!is_valid(XorValue2))
	{
		return 0;
	}

	return XorValue1 ^ mem.Read<uint64_t>(XorValue2 + 0x8 * id);
}

inline uint64_t GetPlayerById(const Memory& mem, int id)
{
	uint64_t pClientGameContext = mem.Read<uint64_t>(b::offsets.game_context);
	if (!is_valid(pClientGameContext))
	{
		return 0;
	}

	uint64_t pPlayerManager = mem.Read<uint64_t>(pClientGameContext + 0x68);
	if (!is_valid(pPlayerManager))
	{
		return 0;
	}

	uint64_t pObfuscationMgr = mem.Read<uint64_t>(b::offsets.obf_mngr);
	if (!is_valid(pObfuscationMgr))
	{
		return 0;
	}

	uint64_t PlayerListXorValue = mem.Read<uint64_t>(pPlayerManager + 0xF8);
	uint64_t PlayerListKey = PlayerListXorValue ^ mem.Read<uint64_t>(pObfuscationMgr + 0x70);

	uint64_t mpBucketArray = mem.Read<uint64_t>(pObfuscationMgr + 0x10);


	int mnBucketCount = mem.Read<int>(pObfuscationMgr + 0x18);
	if (mnBucketCount == 0)
	{
		return 0;
	}

	int startCount = (int)PlayerListKey % mnBucketCount;

	uint64_t mpBucketArray_startCount = mem.Read<uint64_t>(mpBucketArray + (uint64_t)(startCount * 8));
	uint64_t node_first = mem.Read<uint64_t>(mpBucketArray_startCount);
	uint64_t node_second = mem.Read<uint64_t>(mpBucketArray_startCount + 0x8);
	uint64_t node_mpNext = mem.Read<uint64_t>(mpBucketArray_startCount + 0x10);

	while (PlayerListKey != node_first)
	{
		mpBucketArray_startCount = node_mpNext;

		node_first = mem.Read<uint64_t>(mpBucketArray_startCount);
		node_second = mem.Read<uint64_t>(mpBucketArray_startCount + 0x8);
		node_mpNext = mem.Read<uint64_t>(mpBucketArray_startCount + 0x10);
	}

	uint64_t EncryptedPlayerMgr = node_second;
	return EncryptedPlayerMgr_GetPlayer(mem, EncryptedPlayerMgr, id);
}

inline uint64_t GetLocalPlayer(const Memory& mem)
{
	uint64_t AAAAAAAAAA = mem.Read<uint64_t>(b::offsets.game_context);
	if (!is_valid(AAAAAAAAAA))
	{
		return 1;
	}

	uint64_t pPlayerManager = mem.Read<uint64_t>(AAAAAAAAAA + 0x68);
	if (!is_valid(pPlayerManager))
	{
		return 1;
	}

	uint64_t pObfuscationMgr = mem.Read<uint64_t>(b::offsets.obf_mngr);
	if (!is_valid(pObfuscationMgr))
	{
		return 1;
	}

	uint64_t PlayerListXorValue = mem.Read<uint64_t>(pPlayerManager + 0xF8);
	uint64_t PlayerListKey = PlayerListXorValue ^ mem.Read<uint64_t>(pObfuscationMgr + 0x70);

	uint64_t mpBucketArray = mem.Read<uint64_t>(pObfuscationMgr + 0x10);


	int mnBucketCount = mem.Read<int>(pObfuscationMgr + 0x18);
	if (mnBucketCount == 0)
	{
		return 0;
	}

	int startCount = (int)PlayerListKey % mnBucketCount;

	uint64_t mpBucketArray_startCount = mem.Read<uint64_t>(mpBucketArray + (uint64_t)(startCount * 8));
	uint64_t node_first = mem.Read<uint64_t>(mpBucketArray_startCount);
	uint64_t node_second = mem.Read<uint64_t>(mpBucketArray_startCount + 0x8);
	uint64_t node_mpNext = mem.Read<uint64_t>(mpBucketArray_startCount + 0x10);

	while (PlayerListKey != node_first)
	{
		mpBucketArray_startCount = node_mpNext;

		node_first = mem.Read<uint64_t>(mpBucketArray_startCount);
		node_second = mem.Read<uint64_t>(mpBucketArray_startCount + 0x8);
		node_mpNext = mem.Read<uint64_t>(mpBucketArray_startCount + 0x10);
	}

	uint64_t EncryptedPlayerMgr = node_second;
	return EncryptedPlayerMgr_GetPlayer(mem, EncryptedPlayerMgr, NULL);

}
inline uint64_t GetSoldierWeapon(const Memory& mem)
{
	DWORD64 pSoldierWeapon = 0x0;

	uint64_t pLocalPlayer = GetLocalPlayer(mem);
	if (is_valid(pLocalPlayer)) {
		uint64_t pLocalSoldier = mem.Read<uint64_t>(pLocalPlayer + 0x1D48);
		if (is_valid(pLocalSoldier)) {

			uint64_t pClientWeaponComponent = mem.Read<uint64_t>(pLocalPlayer + 0x698);
			if (is_valid(pClientWeaponComponent)) {

				uint64_t m_handler = mem.Read<uint64_t>(pClientWeaponComponent + 0x08A8);
				uint64_t m_activeSlot = mem.Read<uint64_t>(pClientWeaponComponent + 0x0A10);

				if (is_valid(m_handler))
				{
					pSoldierWeapon = mem.Read<uint64_t>(m_handler + m_activeSlot * 0x8);
				}
			}

		}
	}
	return pSoldierWeapon;
}
