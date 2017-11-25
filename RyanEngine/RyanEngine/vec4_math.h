#pragma once
#include "universal.h"
#include "types.h"

struct vec4
{
	union
	{
		__m128 mmval;
		float_t	ALIGN(16) ptr[4];
		struct { float_t x, y, z, w; };
	};

	vec4(const float* val, bool aligned)
	{
		if(aligned)
			mmval = _mm_load_ps(val);
		else
			mmval = _mm_loadu_ps(val);
	}

	vec4(float x, float y, float z, float w)
	{
		mmval = _mm_setr_ps(x, y, z, w);
	}

	vec4(__m128 val)
	{
		mmval = val;
	}

	vec4(vec4& val)
	{
		mmval = val.mmval;
	}

	vec4(float b)
	{
		mmval = _mm_set1_ps(b);
	}

	vec4()
	{
		mmval = _mm_set1_ps(0.0f);
	}

	operator __m128()
	{
		return mmval;
	}

	inline void copyToFloatAligned(float* out)
	{
		_mm_store_ps(out, mmval);
	}

	inline void copyToFloatUnAligned(float* out)
	{
		_mm_storeu_ps(out, mmval);
	}

	inline float* floatArray()
	{
		return mmval.m128_f32;
	}

	inline vec4 operator+(const vec4& b)
	{
		const __m128 ret = _mm_add_ps(mmval, b.mmval);
		return vec4(ret);
	}

	inline vec4 operator-(const vec4& b)
	{
		const __m128 ret = _mm_sub_ps(mmval, b.mmval);
		return vec4(ret);
	}

	inline void operator+=(const vec4& b)
	{
		mmval = _mm_add_ps(mmval, b.mmval);
	}

	inline void operator-=(const vec4& b)
	{
		mmval = _mm_sub_ps(mmval, b.mmval);
	}

	inline vec4 operator*(const vec4& b)
	{
		const __m128 ret = _mm_mul_ps(mmval, b.mmval);
		return vec4(ret);
	}

	inline vec4 operator*(float b)
	{
		const __m128 mul = _mm_set1_ps(b);
		const __m128 ret = _mm_mul_ps(mmval, mul);
		return vec4(ret);
	}

	inline void operator*=(const vec4& b)
	{
		mmval = _mm_mul_ps(mmval, b.mmval);
	}

	inline void operator*=(float b)
	{
		const __m128 mul = _mm_set1_ps(b);
		mmval = _mm_mul_ps(mmval, mul);
	}

	inline vec4 operator/(const vec4& b)
	{
		const __m128 ret = _mm_div_ps(mmval, b.mmval);
		return vec4(ret);
	}

	inline vec4 operator/(float b)
	{
		const __m128 mul = _mm_set1_ps(b);
		const __m128 ret = _mm_div_ps(mmval, mul);
		return vec4(ret);
	}

	inline void operator/=(const vec4& b)
	{
		mmval = _mm_div_ps(mmval, b.mmval);
	}

	inline void operator/=(float b)
	{
		const __m128 mul = _mm_set1_ps(b);
		mmval = _mm_div_ps(mmval, mul);
	}

	template<int a, int b, int c, int d>
	inline vec4 swizzle()
	{
		const __m128 ret = _mm_permute_ps(mmval, _MM_SHUFFLE(a, b, c, d));
		return vec4(ret);
	}

	inline float dot(const vec4& b)
	{
		return _mm_cvtss_f32( _mm_dp_ps( mmval, b.mmval, 0xF1 ) );
	}

	inline vec4 cross(const vec4& b)
	{
		return _mm_sub_ps(
							_mm_mul_ps(_mm_shuffle_ps(mmval, mmval, _MM_SHUFFLE(0, 2, 1, 3)), _mm_shuffle_ps(b.mmval, b.mmval, _MM_SHUFFLE(1, 0, 2, 3))),
							_mm_mul_ps(_mm_shuffle_ps(mmval, mmval, _MM_SHUFFLE(1, 0, 2, 3)), _mm_shuffle_ps(b.mmval, b.mmval, _MM_SHUFFLE(0, 2, 1, 3)))
						 );
	}

	inline vec4 fma(const vec4& b, const vec4& c)
	{
		const __m128 ret = _mm_fmadd_ps(mmval, b.mmval, c.mmval);
		return vec4(ret);
	}

};

inline vec4 operator*(float b, vec4& v)
{
	return v * b;
}