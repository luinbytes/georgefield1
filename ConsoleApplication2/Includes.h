#pragma once
#include "Memory.h"
#include "Obfuscationmgr.h"
#include "Offsets.h"

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
