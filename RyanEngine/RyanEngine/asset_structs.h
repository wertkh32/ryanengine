#pragma once
#include <stdint.h>

#define MODEL_MAX_LODS	5
#define ASSET_NAME_LENGTH 256

enum GfxShaderType
{
	GFX_VERTEX_SHADER,
	GFX_HULL_SHADER,
	GFX_DOMAIN_SHADER,
	GFX_PIXEL_SHADER,

	GFX_SHADER_TYPE_COUNT,
};


enum GfxVertexType
{
	GFX_VERTEX_XYZ,
	GFX_VERTEX_UV,
	//GFX_VERTEX_NORMAL,
	//GFX_VERTEX_TANGENT,
	GFX_VERTEX_COUNT,
};


struct ShaderAssetRaw
{
	const char name[ASSET_NAME_LENGTH];
	GfxShaderType type;
	// no shaders yet. use defaults
};


struct TextureAssetRaw
{
	const char name[ASSET_NAME_LENGTH];
	uint32_t width;
	uint32_t height;
	uint32_t pixelFormat;
	uint32_t pixelSize;

	uint32_t textureDataOffset;
};


struct MaterialAssetRaw
{
	uint32_t numShaders;
	uint32_t numTextures;

	uint32_t shadersDataOffset;		//relative offset to ShaderAssetRaw
	uint32_t texturesDataOffset;	//relative offset to TextureAssetRaw
};


struct SurfaceAssetRaw
{
	uint32_t vertexCount;
	uint32_t indexCount;
	
	uint32_t materialDataOffset;	//relative offset to MaterialAssetRaw

	uint32_t vertexBufferDataOffset[GFX_VERTEX_COUNT];
	uint32_t indexBufferDataOffset;
};


struct ModelLODAssetRaw
{
	uint32_t numSurfaces;
	uint32_t surfacesDataOffset;	//relative offset to an array of SurfaceAssetRaws
};


struct ModelAssetRaw
{
	const char name[ASSET_NAME_LENGTH];
	
	uint32_t numLods;
	uint32_t modelLODsDataOffset;	//relative offset to an array of ModelLODAssetRaws
};


enum ASSET_TYPE
{
	ASSET_TYPE_MODEL,
	ASSET_TYPE_SHADER,
	ASSET_TYPE_TEXTURE,

	ASSET_TYPE_COUNT,
};


enum ASSET_POOL_SIZE : uint32_t
{
	ASSET_POOL_MODEL_SIZE = 256,
	ASSET_POOL_SHADER_SIZE = 256,
	ASSET_POOL_TEXTURE_SIZE = 256,
};