#pragma once
#include "universal.h"
#include "gfx_globals.h"

#define MAX_STATIC_RESOURCES 128
#define MAX_DYNAMIC_RESOURCES 128
#define BUFFER_HEAP_SIZE ( 2 * MB )
#define UPLOAD_HEAP_SIZE ( 2 * MB )
#define TEXTURE_HEAP_SIZE ( 4 * MB )
#define MEM_MAN_RESOURCE_ALIGNMENT ( 64 * KB ) // memory manager does not support multisampled texture for now

sassert ( MEM_MAN_RESOURCE_ALIGNMENT == GPU_PAGE_SIZE );

enum GFX_RESOURCE_TYPE
{
	GFX_RESOURCE_GPU_ONLY = 0x1,
	GFX_RESOURCE_WRITE_COMBINE = 0x2,
	GFX_RESOURCE_UAV = 0x4, 

	GFX_MEM_TYPE_COUNT = 3
};

#define GPU_RESOURCE_INVALID_ACCESS_MASK ( GFX_RESOURCE_GPU_ONLY | GFX_RESOURCE_WRITE_COMBINE )

sassert ( GFX_MEM_TYPE_COUNT < sizeof ( uint ) * 8 );


// keeps track of all the resources created.
// allows proper clean up when assets are unloaded.
class GfxResourceManager
{
private:
	gfx_device_t  *gfxDevice;

	uint staticResourceCount;
	uint dynamicResourceCount;

	gfx_heap_t	  *bufferHeap;
	gfx_heap_t	  *uploadHeap;
	gfx_heap_t	  *textureHeap;

	uint		  bufferHeapOffset;
	uint		  uploadHeapOffset;
	uint		  textureHeapOffset;

	gfx_resource_t *staticResources[MAX_STATIC_RESOURCES];
	gfx_resource_t *dynamicResources[MAX_DYNAMIC_RESOURCES];

	GfxResourceManager ();
	
	~GfxResourceManager ();

	bool checkTextureResourceValid ( GFX_TEXTURE_FORMAT format, bool useMips );
	
	uint getResourceAllocSize ( const gfx_resource_desc_t *resourceDesc )
	{
		D3D12_RESOURCE_ALLOCATION_INFO allocInfo = gfxDevice->GetResourceAllocationInfo ( 0, 1, resourceDesc );

		assert ( allocInfo.Alignment == static_cast<UINT64>(MEM_MAN_RESOURCE_ALIGNMENT) );

		return static_cast< uint >( allocInfo.SizeInBytes );
	}

public:
	static GfxResourceManager* Instance ()
	{
		static GfxResourceManager* memMan = 0;
		// instantiate only once... no more
		if ( !memMan )
			memMan = new GfxResourceManager ();
		return memMan;
	}

	void Init ( gfx_device_t *_gfxDevice );

	gfx_resource_t *allocateBuffer ( const gfx_resource_desc_t *desc, uint resourceFlags );

	gfx_resource_t *allocateTexture2D ( uint width, uint height, gfx_resource_state_t state, 
										GFX_TEXTURE_FORMAT format, uint mipLevels = 0 ); // mipLevels == 0 -> populate all mips

	void deallocateAllDynamicResources (); // this is called when a scene is shutdown
	
	void deallocateAllStaticResources (); // this is called when the program shutsdown

};

