#include "SceneLoader.h"
#include <stdio.h>
#include <string.h>
#include "StackAllocator.h"


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


void SceneLoader::FixupSceneFromSceneRaw ()
{
	// there is probably a automated way to handle this translation
	assert ( scene );
	assert ( sceneMemBlob );

	const uintptr_t memPtrStart = reinterpret_cast< const uintptr_t >( sceneMemBlob );

	scene->models			= reinterpret_cast< ModelAsset* >(		reinterpret_cast< uintptr_t >( scene->models )				+ memPtrStart );
	scene->surfaces			= reinterpret_cast< SurfaceAsset* >(	reinterpret_cast< uintptr_t >( scene->surfaces )			+ memPtrStart );
	scene->materials		= reinterpret_cast< MaterialAsset* >(	reinterpret_cast< uintptr_t >( scene->materials )			+ memPtrStart );
	scene->textures			= reinterpret_cast< TextureAsset* >(	reinterpret_cast< uintptr_t >( scene->textures )			+ memPtrStart );
	scene->modelTransforms	= reinterpret_cast< mat4x4* >(			reinterpret_cast< uintptr_t >( scene->modelTransforms )		+ memPtrStart );
	scene->shaders = nullptr; //reinterpret_cast< ShaderAsset* >( reinterpret_cast< uintptr_t >( scene->shaders ) + memPtrStart );

	for( uint32_t modelIndex = 0; modelIndex < scene->numModels; modelIndex++ )
	{
		ModelAsset *model = scene->models + modelIndex;
	}
}


SceneLoader::~SceneLoader ()
{
}
