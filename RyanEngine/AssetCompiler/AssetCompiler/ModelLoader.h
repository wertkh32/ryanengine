#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include "asset_structs.h"

using namespace Assimp;

struct StackAllocator;

class ModelLoader
{
	Importer importer;
	const aiScene* scene;

	void processNode ( aiNode * node );
public:
	void getModelFromAIScene ( ModelAssetRaw *model, StackAllocator *allocator );
	ModelLoader ( const char* fileName );
	~ModelLoader ();
};

