#ifdef COMPILING_ASSETS
#include <stdint.h>
#include "asset_defines.h"
#include "gfx_texture_types.h"
#include "gfx_vertex_streams.h"
#include "mat4x4_math.h"


#if COMPILING_ASSETS
#undef ASSET_STRUCT_NAME
#undef ASSET_DATA_PTR
#define ASSET_STRUCT_NAME( NAME ) NAME##Raw
#define ASSET_DATA_PTR( TYPE, NAME ) uintptr_t NAME##Offset
#else
#undef ASSET_STRUCT_NAME
#undef ASSET_DATA_PTR
#define ASSET_STRUCT_NAME( NAME ) NAME
#define ASSET_DATA_PTR( TYPE, NAME ) TYPE *NAME
#endif


// elementary struct
struct ASSET_STRUCT_NAME( ShaderAsset )
{
	const char name[ASSET_NAME_LENGTH];
	GfxShaderType type;
	// no shaders yet. use defaults
};


// elementary struct
struct ASSET_STRUCT_NAME( TextureAsset )
{
	char name[ASSET_NAME_LENGTH];
	uint32_t width;
	uint32_t height;
	uint32_t pixelFormat;
	uint32_t pixelSize;

	ASSET_DATA_PTR( uint8_t, textureData );
};


// compound struct. made up of shaders and textures
struct ASSET_STRUCT_NAME( MaterialAsset )
{
	ASSET_DATA_PTR( ShaderID, shaderIndices );
	ASSET_DATA_PTR( TextureID, textureIndices );

	uint32_t numShaders;
	uint32_t numTextures;
};


// elementary struct
struct ASSET_STRUCT_NAME( SurfaceAsset )
{
	ASSET_DATA_PTR( uint8_t, vertexBuffer )[GFX_VERTEX_COUNT];
	ASSET_DATA_PTR( index_t, indexBuffer );

	uint32_t vertexCount;
	uint32_t indexCount;
};


// compound struct. made up of surfaces and materials and the relationship between surfaces and materials
struct ASSET_STRUCT_NAME( ModelAsset )
{
	char name[ASSET_NAME_LENGTH];

	ASSET_DATA_PTR( SurfaceID, surfaceIndices );
	ASSET_DATA_PTR( MaterialID, surfaceToMaterialMap );

	uint32_t numSurfaces;
};


struct ASSET_STRUCT_NAME( Scene )
{
	ASSET_DATA_PTR( ASSET_STRUCT_NAME( ModelAsset ), models );
	ASSET_DATA_PTR( ASSET_STRUCT_NAME( SurfaceAsset ), surfaces );
	ASSET_DATA_PTR( ASSET_STRUCT_NAME( MaterialAsset ), materials );
	ASSET_DATA_PTR( ASSET_STRUCT_NAME( ShaderAsset ), shaders );
	ASSET_DATA_PTR( ASSET_STRUCT_NAME( TextureAsset ), textures );
	ASSET_DATA_PTR( mat4x4, modelTransforms );

	uint32_t numModels;
	uint32_t numSurfaces;
	uint32_t numMaterials;
	uint32_t numTextures;
	uint32_t numShaders;
};
#endif // #ifdef COMPILING_ASSETS