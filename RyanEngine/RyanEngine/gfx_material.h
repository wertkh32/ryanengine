#pragma once

#include "universal.h"
#include "gfx_types.h"

typedef uint GfxMaterialFlags;

enum GFX_MATERIAL_FLAG
{
	GFX_MATERIAL_BACKFACE_CULL = 1,
	GFX_MATERIAL_NO_CAST_SHADOW = 2,
};

struct GfxMaterial
{
	const char			*name;
	ID3DBlob		*vertexShader;
	ID3DBlob		*pixelShader;
	GfxMaterialFlags	flags;
};
