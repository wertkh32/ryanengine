#include "SceneLoader.h"
#include <stdio.h>
#include <string.h>
#include "StackAllocator.h"

#define COMPILING_ASSETS 0
#include "asset_structs.h"
#undef COMPILING_ASSETS
#define COMPILING_ASSETS 1
#include "asset_structs.h"

// all of this must be true for fixup to happen.
// also need to write sizeof ptr to file and read from it to verify
sassert ( sizeof ( Scene ) == sizeof ( SceneRaw ) );
sassert ( sizeof ( ModelAsset ) == sizeof ( ModelAssetRaw ) );
sassert ( sizeof ( SurfaceAsset ) == sizeof ( SurfaceAssetRaw ) );
sassert ( sizeof ( MaterialAsset ) == sizeof ( MaterialAssetRaw ) );
sassert ( sizeof ( ShaderAsset ) == sizeof ( ShaderAssetRaw ) );
sassert ( sizeof ( TextureAsset ) == sizeof ( TextureAssetRaw ) );


SceneLoader::SceneLoader( const char* fileName, StackAllocator *inAllocator )
	:	scene( nullptr ),
		sceneMemBlob( nullptr )
{
	FILE* fileHandle = fopen( fileName, "rb" );

	assert( fileHandle );

	fseek( fileHandle, 0, SEEK_END );

	sceneMemBlobSize = static_cast< uint32_t >( ftell( fileHandle ) );

	fseek ( fileHandle, 0, SEEK_SET );

	assert( sceneMemBlobSize <= inAllocator->totalSize );

	byte *allocMem = inAllocator->allocBytes ( sceneMemBlobSize );

	sceneMemBlob = allocMem;
	scene = reinterpret_cast< Scene* >( sceneMemBlob );

	size_t bytesRead = fread ( allocMem, 1, sceneMemBlobSize, fileHandle );

	assert ( bytesRead == static_cast< size_t >( sceneMemBlobSize ) );

	fclose ( fileHandle );
}

SceneLoader::SceneLoader ( byte* memBlob, uint32_t memBlobSize )
	:	scene ( nullptr ),
		sceneMemBlob ( nullptr )
{
	assert ( memBlob );
	assert ( memBlobSize >= sizeof ( Scene ) );

	sceneMemBlob = memBlob;
	scene = reinterpret_cast< Scene* >( sceneMemBlob );
	sceneMemBlobSize = memBlobSize;
}


template <typename TYPE>
void FixupPtr ( TYPE *&ptr, const uintptr_t baseMemPtr )
{
	ptr = reinterpret_cast< TYPE* >( reinterpret_cast< uintptr_t >( ptr ) + baseMemPtr );
}


void SceneLoader::FixupSceneFromSceneRaw ()
{
	// there is probably a automated way to handle this translation
	// need to perform corruption checks
	assert ( scene );
	assert ( sceneMemBlob );

	const uintptr_t memPtrStart = reinterpret_cast< const uintptr_t >( sceneMemBlob );

	FixupPtr( scene->models, memPtrStart );
	FixupPtr ( scene->surfaces, memPtrStart );
	FixupPtr( scene->materials, memPtrStart );
	FixupPtr( scene->textures, memPtrStart );
	FixupPtr( scene->modelTransforms, memPtrStart );
	scene->shaders = nullptr; //FixupPtr( scene->shaders, memPtrStart );

	for( uint32_t modelIndex = 0; modelIndex < scene->numModels; modelIndex++ )
	{
		ModelAsset *model = scene->models + modelIndex;
		FixupPtr ( model->surfaceIndices, memPtrStart );
		FixupPtr ( model->surfaceToMaterialMap, memPtrStart );
	}

	for( uint32_t surfIndex = 0; surfIndex < scene->numSurfaces; surfIndex++ )
	{
		SurfaceAsset *surf = scene->surfaces + surfIndex;
		FixupPtr ( surf->indexBuffer, memPtrStart );
		
		for( uint32_t vertexType = 0; vertexType < GFX_VERTEX_COUNT; vertexType++ )
		{
			FixupPtr ( surf->vertexBuffer[vertexType], memPtrStart );
		}
	}


	for( uint32_t materialIndex = 0; materialIndex < scene->numMaterials; materialIndex++ )
	{
		MaterialAsset *mat = scene->materials + materialIndex;
		mat->shaderIndices = nullptr; //FixupPtr ( mat->shaderIndices, memPtrStart );
		FixupPtr ( mat->textureIndices, memPtrStart );
	}


	for( uint32_t textureIndex = 0; textureIndex < scene->numTextures; textureIndex++ )
	{
		TextureAsset *tex = scene->textures + textureIndex;

		FixupPtr ( tex->textureData, memPtrStart );
	}

	printf ( "\nScene Loaded! Scene has %d models\n", scene->numModels );
	printf ( "%d surfaces\n", scene->numSurfaces );
	printf ( "%d materials\n", scene->numMaterials );
	printf ( "%d textures\n", scene->numTextures );
}


SceneLoader::~SceneLoader ()
{
}
