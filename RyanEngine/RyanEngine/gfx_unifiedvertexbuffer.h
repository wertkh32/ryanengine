#pragma once
#include "universal.h"
#include "gfx_types.h"

#define VERTEX_MEMORY_SIZE ( 2 * MB )
#define VERTEX_BLOCK_SIZE  ( 128 )
#define VERTEX_BLOCK_COUNT ( VERTEX_MEMORY_SIZE / VERTEX_BLOCK_SIZE )


enum GFX_VERTEX_BUFFER
{
	GFX_POSITION_VERTEX_BUFFER,
	GFX_COLOR_VERTEX_BUFFER,

	GFX_VERTEX_BUFFER_COUNT
};


class GfxUnifiedVertexBuffer
{
	GfxVertBuffer vertBuf[GFX_VERTEX_BUFFER_COUNT];
	byte *curPtrs[GFX_VERTEX_BUFFER_COUNT];
	uint allocated;


public:
	GfxUnifiedVertexBuffer ();
	~GfxUnifiedVertexBuffer ();

	uint allocate ( uint numElements );
	byte *getPtrToVertexBuffer ( GFX_VERTEX_BUFFER vertexBuffer, uint offset );
};

