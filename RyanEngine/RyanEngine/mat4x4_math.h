#pragma once
#include "universal.h"
#include "vec4_math.h"

struct mat4x4
{
	vec4 row[4];

	mat4x4()
	{
		row[0] = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);
		row[1] = _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f);
		row[2] = _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f);
		row[3] = _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f);
	}

	mat4x4(mat4x4& m)
	{
		row[0] = m.row[0];
		row[1] = m.row[1];
		row[2] = m.row[2];
		row[3] = m.row[3];
	}

	mat4x4(const vec4& _row0, const vec4& _row1, const vec4& _row2, const vec4& _row3)
	{
		row[0] = _row0;
		row[1] = _row1;
		row[2] = _row2;
		row[3] = _row3;
	}

	mat4x4(const __m128& _row0, const __m128& _row1, const __m128& _row2, const __m128& _row3)
	{
		row[0].mmval = _row0;
		row[1].mmval = _row1;
		row[2].mmval = _row2;
		row[3].mmval = _row3;
	}

	mat4x4(float _00, float _01, float _02, float _03,
		   float _10, float _11, float _12, float _13,
		   float _20, float _21, float _22, float _23, 
		   float _30, float _31, float _32, float _33)
	{
		row[0] = _mm_set_ps(_00, _01, _02, _03);
		row[1] = _mm_set_ps(_10, _11, _12, _13);
		row[2] = _mm_set_ps(_20, _21, _22, _23);
		row[3] = _mm_set_ps(_30, _31, _32, _33);
	}

	mat4x4 transpose()
	{
		__m128 tmp0 = _mm_unpacklo_ps(row[0], row[1]);
		__m128 tmp1 = _mm_unpackhi_ps(row[0], row[1]);
		__m128 tmp2 = _mm_unpacklo_ps(row[2], row[3]);
		__m128 tmp3 = _mm_unpackhi_ps(row[2], row[3]);

		__m128 ret0 = _mm_movelh_ps(tmp0, tmp1);
		__m128 ret1 = _mm_movehl_ps(tmp0, tmp1);
		
		__m128 ret2 = _mm_movelh_ps(tmp2, tmp3);
		__m128 ret3 = _mm_movehl_ps(tmp2, tmp3);

		return mat4x4(ret0, ret1, ret2, ret3);
	}

	vec4 operator*(const vec4& val)
	{
		__m128 ret = _mm_setzero_ps();
		ret = _mm_add_ps( ret, _mm_dp_ps( row[0].mmval, val.mmval, 0xF1 ) );
		ret = _mm_add_ps( ret, _mm_dp_ps( row[1].mmval, val.mmval, 0xF2 ) );
		ret = _mm_add_ps( ret, _mm_dp_ps( row[2].mmval, val.mmval, 0xF4 ) );
		ret = _mm_add_ps( ret, _mm_dp_ps( row[3].mmval, val.mmval, 0xF8 ) );
		return vec4(ret);
	}

	mat4x4 operator*(mat4x4& m)
	{
		//find a faster way later
		mat4x4 tmp = m.transpose();
		vec4 ret0 = *this * tmp.row[0];
		vec4 ret1 = *this * tmp.row[1];
		vec4 ret2 = *this * tmp.row[2];
		vec4 ret3 = *this * tmp.row[3];

		return mat4x4( ret0, ret1, ret2, ret3 );
	}
};