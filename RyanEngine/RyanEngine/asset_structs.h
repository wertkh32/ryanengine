#pragma once
#include "universal.h"

struct ModelAsset
{
	const char *name;
};


enum GfxShaderType
{
	GFX_VERTEX_SHADER,
	GFX_HULL_SHADER,
	GFX_DOMAIN_SHADER,
	GFX_PIXEL_SHADER,

	GFX_SHADER_TYPE_COUNT,
};


struct ShaderAsset
{
	const char *name;
	GfxShaderType type;
};


struct TextureAsset
{
	const char *name;
	uint width;
	uint height;
};