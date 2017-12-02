#pragma once
#include "universal.h"
#include "gfx_types.h"

#define MAX_STATIC_RESOURCES 128
#define MAX_DYNAMIC_RESOURCES 128
#define BUFFER_HEAP_SIZE ( 2 * MB )
#define UPLOAD_HEAP_SIZE ( 2 * MB )
#define TEXTURE_HEAP_SIZE ( 4 * MB )

enum GFX_MEM_TYPE
{
	GFX_MEM_GPU_ONLY = 1U,
	GFX_MEM_WRITE_COMBINE = 2U,

	GFX_MEM_TYPE_COUNT
};

sassert ( GFX_MEM_TYPE_COUNT < sizeof ( uint ) * 8 );


// keeps track of all the resources created.
// allows proper clean up when assets are unloaded.
class GfxMemManager
{
	uint totalMemoryUsed;
	uint totalStaticResourceMem;
	uint totalDynamicResourceMem;

	uint staticResourceCount;
	uint dynamicResourceCount;

	gfx_device_t  *gfxDevice;

	gfx_heap_t	  *bufferHeap;
	gfx_heap_t	  *uploadHeap;
	gfx_heap_t	  *textureHeap;

	uint		  bufferHeapOffset;
	uint		  uploadHeapOffset;
	uint		  textureHeapOffset;

	gfx_resource_t *staticResources[MAX_STATIC_RESOURCES];
	gfx_resource_t *dynamicResources[MAX_DYNAMIC_RESOURCES];
	
public:
	GfxMemManager ( gfx_device_t *_gfxDevice );
	~GfxMemManager ();

	gfx_resource_t *allocateConstantBuffer ( uint size, gfx_resource_state_t state, uint flags );
	gfx_resource_t *allocateVertexBuffer ( uint size, gfx_resource_state_t state, uint flags );
	gfx_resource_t *allocateIndexBuffer ( uint size, gfx_resource_state_t state, uint flags );
	gfx_resource_t *allocateTexture ( uint size, gfx_resource_state_t state, uint flags );

	void deallocateAllDynamicResources (); // this is called when a scene is shutdown
	void deallocateAllStaticResources (); // this is called when the program shutsdown

};

