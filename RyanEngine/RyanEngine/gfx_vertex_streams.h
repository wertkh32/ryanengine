#pragma once
#include "types.h"

#define MESH_MAX_VERTICES 65536 // because we are using 16 bit indices

enum GfxVertexType
{
	GFX_VERTEX_XYZ,
	GFX_VERTEX_UV,
	GFX_VERTEX_NORMAL,
	GFX_VERTEX_TANGENT,

	GFX_VERTEX_COUNT,
};


typedef float3	pos_t;		//	GFX_VERTEX_XYZ
typedef float2	uv_t;		//	GFX_VERTEX_UV,
typedef float3	axis_t;		//	GFX_VERTEX_NORMAL, GFX_VERTEX_TANGENT


enum GfxVertexElemSize
{
	GFX_VERTEX_XYZ_SIZE = sizeof( pos_t ),
	GFX_VERTEX_UV_SIZE = sizeof( uv_t ),

	//to do: replace normal and tangent with a quaternion asap
	// or experiment with polar coordinates
	GFX_VERTEX_NORMAL_SIZE = sizeof( axis_t ),
	GFX_VERTEX_TANGENT_SIZE = sizeof( axis_t ),
	
	GFX_VERTEX_SIZE_COUNT,
};


typedef uint16_t index16_t;
typedef uint32_t index32_t;
typedef index16_t index_t;