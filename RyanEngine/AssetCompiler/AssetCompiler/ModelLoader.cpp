#include "ModelLoader.h"
#include "StackAllocator.h"
#include "Stack.h"
#include <assert.h>

#define MAX_NODES 64

ModelLoader::ModelLoader ( const char* fileName )
{
	scene = importer.ReadFile ( fileName,
		aiProcessPreset_TargetRealtime_Fast |
		aiProcess_ConvertToLeftHanded |
		aiProcess_PreTransformVertices );

	assert ( scene );
}


void ModelLoader::processNode ( aiNode * node )
{
	return;
}

void ModelLoader::getModelFromAIScene ( ModelAssetRaw *model, StackAllocator *allocator )
{
	ModelAssetRaw *modelAsset;
	ModelLODAssetRaw *modelLODAsset;
	SurfaceAssetRaw *surfaceAssets;
	
	uint32_t baseOffset;

	baseOffset = allocator->currentSize;

	modelAsset = reinterpret_cast< ModelAssetRaw* >( allocator->allocBytes ( sizeof ( ModelAssetRaw ) ) );
	modelAsset->numLods = 1; 	//Only 1 lod for now
	modelAsset->modelLODsDataOffset = allocator->currentSize - baseOffset;
	
	modelLODAsset = reinterpret_cast< ModelLODAssetRaw* >( allocator->allocBytes ( sizeof ( ModelLODAssetRaw ) ) );
	modelLODAsset->numSurfaces = scene->mNumMeshes;
	modelLODAsset->surfacesDataOffset = allocator->currentSize - baseOffset;

	surfaceAssets = reinterpret_cast< SurfaceAssetRaw* >( allocator->allocBytes ( sizeof ( SurfaceAssetRaw ) * modelLODAsset->numSurfaces ) );

	for ( uint32_t i = 0; i < scene->mNumMeshes; i++ )
	{
		aiMesh* mesh = scene->mMeshes[i];
		SurfaceAssetRaw &surf = surfaceAssets[i];

		for( uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++ )
			assert ( mesh->mFaces[faceIndex].mNumIndices == 3 ); // must be triangles.

		surf.vertexCount = mesh->mNumVertices;
		surf.indexCount = mesh->mNumFaces * 3;

		// to-do: create a shared vertex header with engine for vertex types and index types
		// stick that through here

	}
	
}



ModelLoader::~ModelLoader ()
{
	delete scene;
}
