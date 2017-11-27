#pragma once
#include "feature.h"

#if USE( DX12_API )
#include "gfx_dx12_types.h"
#endif

struct GfxVertBuffer
{
	gfx_resource_t *buffer;
	gfx_vert_buffer_view_t bufferView;
	byte *mappedPtr;
};


struct GfxIndexBuffer
{
	gfx_resource_t *buffer;
	gfx_index_buffer_view_t bufferView;
	byte *mappedPtr;
};


struct GfxConstantBuffer
{
	gfx_resource_t *buffer;
	byte *mappedPtr;
};