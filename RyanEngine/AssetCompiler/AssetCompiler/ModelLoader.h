#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include "asset_structs.h"

using namespace Assimp;

struct StackAllocator;

#define MODEL_PATH_LEN 256U

class ModelLoader
{
	Importer importer;
	const aiScene* scene;
	char modelFilePath[MODEL_PATH_LEN];
	StackAllocator *allocator;
	uint32_t baseOffset;

	bool isTextureOnDisk ( const char* filePath );
	bool processTextureAsBC1 ( rgba_t *inputTexture, uint32_t width, uint32_t height );
	uint32_t getCurrentOffset ();

	template< typename T >
	T *allocateFileMem ( uint32_t numElems );


public:
	void getModelFromAIScene ();
	ModelLoader ( const char* fileName, StackAllocator *alloc );
	~ModelLoader ();
};

template<typename T>
inline T * ModelLoader::allocateFileMem ( uint32_t numElems )
{
	return reinterpret_cast< T* >( allocator->allocBytes ( sizeof ( T ) * numElems ) );
}
