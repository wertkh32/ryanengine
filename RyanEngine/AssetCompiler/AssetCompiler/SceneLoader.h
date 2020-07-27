#pragma once

#define COMPILING_ASSETS 0
#include "asset_structs.h"
#undef COMPILING_ASSETS
#define COMPILING_ASSETS 1
#include "asset_structs.h"

#include "Stack.h"

struct StackAllocator;

// all of this must be true for fixup to happen.
// also need to write sizeof ptr to file and read from it to verify
sassert ( sizeof ( Scene ) == sizeof ( SceneRaw ) );
sassert ( sizeof ( ModelAsset ) == sizeof ( ModelAssetRaw ) );
sassert ( sizeof ( SurfaceAsset ) == sizeof ( SurfaceAssetRaw ) );
sassert ( sizeof ( MaterialAsset ) == sizeof ( MaterialAssetRaw ) );
sassert ( sizeof ( ShaderAsset ) == sizeof ( ShaderAssetRaw ) );
sassert ( sizeof ( TextureAsset ) == sizeof ( TextureAssetRaw ) );

class SceneLoader
{
	uint32_t sceneMemBlobSize;
	byte* sceneMemBlob;
	Scene *scene;
	SceneRaw *sceneRaw;

public:
	void FixupSceneFromSceneRaw ();
	SceneLoader ( const char* fileName, StackAllocator *inAllocator );
	SceneLoader ( byte* memBlob, uint32_t memBlobSize );
	~SceneLoader ();
};

