#pragma once
//place global structures for rendering here

#include "feature.h"
#include "types.h"
#include "gfx_types.h"

#define MAX_TEXTURE_DIM	4096
#define GFX_TEXTURE_ALIGNMENT			( 64 * KB )
#define GFX_MSAA_TEXTURE_ALIGNMENT		( 4 * MB )
#define GFX_BUFFER_ALIGNMENT			( 64 * KB )
#define GFX_SMALL_RESOURCE_ALIGNMENT	( 4 * KB ) // have to explicitly request in res desc
#define GFX_CONSTANT_BUFFER_ALIGNMENT	( 256U )
#define GPU_PAGE_SIZE					( 64 * KB )