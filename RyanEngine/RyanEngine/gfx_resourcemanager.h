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

enum ID3D12ResourceYPE
{
	GFX_RESOURCE_WRITE_COMBINE = 0x1,
	GFX_RESOURCE_UAV = 0x2,

	GFX_MEM_TYPE_COUNT = 2
};

#define GPU_RESOURCE_INVALID_ACCESS_MASK ( GFX_RESOURCE_GPU_ONLY | GFX_RESOURCE_WRITE_COMBINE )

sassert ( GFX_MEM_TYPE_COUNT < sizeof ( uint ) * 8 );


// keeps track of all the resources created.
// allows proper clean up when assets are unloaded.
class GfxResourceManager : public Singleton<GfxResourceManager>
{
	friend Singleton<GfxResourceManager>;
private:
	ID3D12Device  *gfxDevice;

	uint staticResourceCount;
	uint dynamicResourceCount;

	ID3D12Heap	  *bufferHeap;
	ID3D12Heap	  *uploadHeap;
	ID3D12Heap	  *textureHeap;

	uint		  bufferHeapOffset;
	uint		  uploadHeapOffset;
	uint		  textureHeapOffset;

	ID3D12Resource *staticResources[MAX_STATIC_RESOURCES];
	ID3D12Resource *dynamicResources[MAX_DYNAMIC_RESOURCES];
	
	uint getResourceAllocSize ( const D3D12_RESOURCE_DESC *resourceDesc )
	{
		D3D12_RESOURCE_ALLOCATION_INFO allocInfo = gfxDevice->GetResourceAllocationInfo ( 0, 1, resourceDesc );

		assert ( allocInfo.Alignment == static_cast<UINT64>(MEM_MAN_RESOURCE_ALIGNMENT) );

		return static_cast< uint >( allocInfo.SizeInBytes );
	}

	GfxResourceManager ();
	~GfxResourceManager ();
public:
	void Init ( ID3D12Device *_gfxDevice );

	ID3D12Resource *allocateBuffer ( const D3D12_RESOURCE_DESC *desc, uint resourceFlags );

	ID3D12Resource *allocateTexture ( const D3D12_RESOURCE_DESC *textureDesc, uint resourceFlags );

	void deallocateAllDynamicResources (); // this is called when a scene is shutdown
	
	void deallocateAllStaticResources (); // this is called when the program shutsdown

};

