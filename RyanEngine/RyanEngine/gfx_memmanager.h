#pragma once
#include "universal.h"
#include "gfx_globals.h"

#define MAX_STATIC_RESOURCES 128
#define MAX_DYNAMIC_RESOURCES 128
#define BUFFER_HEAP_SIZE ( 2 * MB )
#define UPLOAD_HEAP_SIZE ( 2 * MB )
#define TEXTURE_HEAP_SIZE ( 4 * MB )
#define MEM_MAN_RESOURCE_ALIGNMENT ( 64 * KB ) // memory manager does not support multisampled texture for now

enum GFX_MEM_TYPE
{
	GFX_MEM_GPU_ONLY = 1U,
	GFX_MEM_WRITE_COMBINE = 2U,

	GFX_MEM_TYPE_COUNT = 2
};


enum GFX_RESOURCE_ACCESS
{
	GFX_RESOURCE_READ_ONLY,
	GFX_RESOURCE_READ_WRITE,

	GFX_RESOURCE_ACCESS_COUNT
};

sassert ( GFX_MEM_TYPE_COUNT < sizeof ( uint ) * 8 );


// keeps track of all the resources created.
// allows proper clean up when assets are unloaded.
class GfxMemManager
{
private:
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

	GfxMemManager ();
	
	~GfxMemManager ();

	bool checkTextureResourceValid ( GFX_TEXTURE_FORMAT format, bool useMips );
	
	uint getResourceAllocSize ( gfx_resource_desc_t *resourceDesc )
	{
		D3D12_RESOURCE_ALLOCATION_INFO allocInfo = gfxDevice->GetResourceAllocationInfo ( 0, 1, resourceDesc );

		assert ( allocInfo.Alignment == static_cast<UINT64>(MEM_MAN_RESOURCE_ALIGNMENT) );

		return static_cast< uint >( allocInfo.SizeInBytes );
	}

public:
	static GfxMemManager* Instance ()
	{
		static GfxMemManager* memMan = 0;
		// instantiate only once... no more
		if ( !memMan )
			memMan = new GfxMemManager ();
		return memMan;
	}

	void Init ( gfx_device_t *_gfxDevice );
	
	gfx_resource_t *allocateConstantBuffer ( uint size, gfx_resource_state_t state, uint memFlags );
	
	gfx_resource_t *allocateRawBuffer ( uint size, gfx_resource_state_t state, uint flags );
	
	gfx_resource_t *allocateTypedBuffer ( uint size, gfx_resource_state_t state, uint flags );
	
	gfx_resource_t *allocateStructuredBuffer ( uint size, gfx_resource_state_t state, uint flags );
	
	gfx_resource_t *allocateVertexBuffer ( uint size, gfx_resource_state_t state, uint flags );
	
	gfx_resource_t *allocateIndexBuffer ( uint size, gfx_resource_state_t state, uint flags );
	
	gfx_resource_t *allocateTexture2D ( uint width, uint height, gfx_resource_state_t state, GFX_RESOURCE_ACCESS access, 
										GFX_TEXTURE_FORMAT format, uint mipLevels = 0 ); // mipLevels == 0 -> populate all mips

	void deallocateAllDynamicResources (); // this is called when a scene is shutdown
	
	void deallocateAllStaticResources (); // this is called when the program shutsdown

};

