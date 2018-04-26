#pragma once
#include "types.h"
#include "universal.h"

#define MODEL_MAX_LODS	5

struct ModelAsset
{
	const char *name;
	uint8_t lodVertexCount[MODEL_MAX_LODS];
	uint8_t lodTriCount[MODEL_MAX_LODS];
	uint8_t numLods;
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


enum ASSET_TYPE
{
	ASSET_TYPE_MODEL,
	ASSET_TYPE_SHADER,
	ASSET_TYPE_TEXTURE,

	ASSET_TYPE_COUNT,
};
sassert ( ASSET_TYPE_COUNT <= 256 );


enum ASSET_POOL_SIZE : uint
{
	ASSET_POOL_MODEL_SIZE = 256,
	ASSET_POOL_SHADER_SIZE = 256,
	ASSET_POOL_TEXTURE_SIZE = 256,
};