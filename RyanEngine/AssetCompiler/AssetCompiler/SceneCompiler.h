#pragma once
#define COMPILING_ASSETS 1

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include "asset_structs.h"
#include "mat4x4_math.h"
#include "Stack.h"

using namespace Assimp;

struct StackAllocator;

#define MODEL_PATH_LEN 256U

class SceneCompiler
{
	Importer importer;
	const aiScene* scene;
	char sceneFilePath[MODEL_PATH_LEN];
	StackAllocator *allocator;
	uint32_t baseOffset;

	StackArray<const aiNode*, MAX_MODELS> modelStack;
	StackArray<mat4x4, MAX_MODELS> transformStack;

	bool isTextureOnDisk ( const char* filePath );
	bool processTextureAsBC1 ( rgba_t *inputTexture, uint32_t width, uint32_t height );
	uint32_t getCurrentOffset ();

	template< typename T >
	T *allocateFileMem ( uint32_t numElems );

	void ExtractModelsFromScene();
	uint GetSceneTextureCount();

public:
	void getModelFromAIScene ();
	SceneCompiler ( const char* fileName, StackAllocator *alloc );
	~SceneCompiler ();
};

template<typename T>
inline T * SceneCompiler::allocateFileMem ( uint32_t numElems )
{
	return reinterpret_cast< T* >( allocator->allocBytes ( sizeof ( T ) * numElems ) );
}
