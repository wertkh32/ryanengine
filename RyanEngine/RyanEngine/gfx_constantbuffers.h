#pragma once
#include "universal.h"
#include "gfx_globals.h"
#include "gfx_math.h"

typedef gfx_resource_t		gfx_cbuffer;

//constant buffers must be multiples of 16
struct gfx_transform_buffer_t
{
	mat4x4 worldMatrix;
	mat4x4 viewMatrix;
	mat4x4 projectionMatrix;
};

static_assert(sizeof(mat4x4) == 64, "mat4x4 is supposed to be 64 bytes");
static_assert( ( sizeof( gfx_transform_buffer_t ) & 15 ) == 0, "constant buffers must be multiples of 16" );


//keep in sync with slot in root signature
enum GFX_CONSTANT_BUFFER
{
	GFX_TRANSFORM_CONSTANT_BUFFER,

	GFX_CONSTANT_BUFFER_COUNT,
};


enum GFX_CONSTANT_BUFFER_SIZE
{
	GFX_TRANSFORM_CONSTANT_BUFFER_SIZE = sizeof( gfx_transform_buffer_t ),
};


const uint g_constantBufferSizes[GFX_CONSTANT_BUFFER_COUNT] = 
{
	GFX_TRANSFORM_CONSTANT_BUFFER_SIZE
};


void Gfx_CreateConstantBuffer( gfx_device_t *gfxDevice, gfx_cbuffer **cbuffer, gfx_desc_handle_t descHandle, uint size );