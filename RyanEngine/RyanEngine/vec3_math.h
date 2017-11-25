#pragma once
#include "universal.h"

struct vec3
{
	union {
		struct { float x, y, z; };
		struct { float ptr[3]; };
	};
	vec3(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	vec3(float v[3])
	{
		x = v[0];
		y = v[1];
		z = v[2];
	}

	vec3(const vec3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	vec3(void)
	{
		x = y = z = 0;
	}

	inline float dot(vec3& v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline vec3 cross(vec3& v)
	{
		return vec3(y * v.z - v.y * z, z * v.x - v.z * x, x * v.y - v.x * y);
	}

	inline vec3 unit()
	{
		float m = mag();
		return vec3(x / m, y / m, z / m);
	}

	inline float mag()
	{
		return sqrt(x*x + y*y + z*z);
	}

	inline vec3& operator=(vec3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	inline vec3 operator*(float k) const
	{
		return vec3(x*k, y*k, z*k);
	}

	inline vec3 operator*(const vec3& v) const
	{
		return vec3(x*v.x, y*v.y, z*v.z);
	}

	inline vec3 operator/(float k) const
	{
		return vec3(x / k, y / k, z / k);
	}

	inline vec3 operator+(const vec3& v) const
	{
		return vec3(x + v.x, y + v.y, z + v.z);
	}

	inline vec3 operator-(const vec3& v) const
	{
		return vec3(x - v.x, y - v.y, z - v.z);
	}

	inline vec3 operator-()
	{
		return vec3(-x, -y, -z);
	}

	inline void operator+= (vec3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}

	inline void operator-= (vec3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}

	inline void operator*= (vec3& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
	}

	inline void operator*= (float k)
	{
		x *= k;
		y *= k;
		z *= k;
	}

	inline void operator/= (vec3& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
	}

	inline void operator/= (float k)
	{
		x /= k;
		y /= k;
		z /= k;
	}

	static vec3 lerp(vec3& start, vec3& end, float t)
	{
		return (start*(1 - t) + end*t);
	}

	static vec3 nlerp(vec3& start, vec3& end, float t)
	{
		return lerp(start, end, t).unit();
	}

	//start and end are unit vectors
	static vec3 slerp(vec3& start, vec3& end, float t)
	{
		float innerp = start.dot(end);
		innerp = CLAMP(innerp, -1, 1);
		float angle = acos(innerp) * t;
		vec3 base = (end - start * innerp).unit();
		return start * cos(angle) + base * sin(angle);
	}

};

inline vec3 operator*(float k, const vec3& v)
{
	return v * k;
}