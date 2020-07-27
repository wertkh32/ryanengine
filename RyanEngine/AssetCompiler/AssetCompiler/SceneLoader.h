#pragma once
#include "types.h"
#include "Stack.h"

struct StackAllocator;
struct Scene;

class SceneLoader
{
	uint32_t sceneMemBlobSize;
	byte* sceneMemBlob;
	Scene *scene;

public:
	void FixupSceneFromSceneRaw ();
	SceneLoader ( const char* fileName, StackAllocator *inAllocator );
	SceneLoader ( byte* memBlob, uint32_t memBlobSize );
	~SceneLoader ();
};

