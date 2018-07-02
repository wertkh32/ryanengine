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

	bool isTextureOnDisk ( const char* filePath );
	void processNode ( aiNode * node );
public:
	void getModelFromAIScene ( StackAllocator *allocator );
	ModelLoader ( const char* fileName );
	~ModelLoader ();
};

