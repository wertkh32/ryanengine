#pragma once
#include "universal.h"
#include "mat4x4_math.h"
#include "vec4_math.h"

#define CORRECTION_EPSILON 2.4e-7f

static mat4x4 Gfx_CalculateProjectionMatrix( float _near, float _width, float _height, float _FOVinRadians)
{
	// infinite projection matrix (from slides by eric lengyel)
	float focalLength = 1.0f / tan(_FOVinRadians * 0.5f);
	float heightToWidthRatio = _height / _width;

	return mat4x4(focalLength, 0.0f, 0.0f, 0.0f,
				  0.0f, focalLength / heightToWidthRatio, 0.0f, 0.0f,
				  0.0f, 0.0f, CORRECTION_EPSILON - 1.0f, (CORRECTION_EPSILON - 2.0f) * _near,
				  0.0f, 0.0f, -1.0f, 0.0f);
}
