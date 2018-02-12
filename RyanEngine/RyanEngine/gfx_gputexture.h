#pragma once
#include "universal.h"
#include "gfx_globals.h"
#include "gfx_resourcemanager.h"

struct GfxGpuTexture
{
	GfxGpuTexture ( uint width, uint height, gfx_resource_state_t state, GFX_TEXTURE_FORMAT format, uint mipLevels  );
	~GfxGpuTexture ();
};
