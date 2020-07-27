#pragma once
#include <stdint.h>

#define ASSET_NAME_LENGTH 256
#define INVALID_DATA_OFFSET (~0U)

// limits are plus one. so really e.g. model limit is MAX_MODELS - 1.
#define MAX_MODELS		1024 // up to 65536
#define MAX_SURFACES	65536
#define MAX_MATERIALS	65536
#define MAX_TEXTURES	65536
#define MAX_SHADERS		65536

typedef uint16_t ModelID;
typedef uint16_t SurfaceID;
typedef uint16_t MaterialID;
typedef uint16_t TextureID;
typedef uint16_t ShaderID;


enum GfxShaderType
{
	GFX_VERTEX_SHADER,
	GFX_HULL_SHADER,
	GFX_DOMAIN_SHADER,
	GFX_PIXEL_SHADER,

	GFX_SHADER_TYPE_COUNT,
};


enum ASSET_TYPE
{
	ASSET_TYPE_MODEL,
	ASSET_TYPE_SURFACE,
	ASSET_TYPE_MATERIAL,
	ASSET_TYPE_SHADER,
	ASSET_TYPE_TEXTURE,

	ASSET_TYPE_COUNT,
};


enum ASSET_POOL_SIZE : uint32_t
{
	ASSET_POOL_MODEL_SIZE = 256,
	ASSET_POOL_SURFACE_SIZE = 256,
	ASSET_POOL_MATERIAL_SIZE = 256,
	ASSET_POOL_SHADER_SIZE = 256,
	ASSET_POOL_TEXTURE_SIZE = 256,
};