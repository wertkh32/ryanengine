#include "gfx_memmanager.h"



GfxMemManager::GfxMemManager ( gfx_device_t *_gfxDevice )
{
	totalMemoryUsed = 0;
	totalStaticResourceMem = 0;
	totalDynamicResourceMem = 0;

	staticResourceCount = 0;
	dynamicResourceCount = 0;

	gfxDevice = _gfxDevice;

	for ( uint i = 0; i < MAX_STATIC_RESOURCES; i++ )
		staticResources[i] = NULL;

	for ( uint i = 0; i < MAX_DYNAMIC_RESOURCES; i++ )
		dynamicResources[i] = NULL;


	// buffer heap with default memory
	gfx_heap_desc_t bufferHeapDesc = CD3DX12_HEAP_DESC ( BUFFER_HEAP_SIZE,
														D3D12_HEAP_TYPE_DEFAULT, 
														D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, 
														D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS );

	DXERROR( gfxDevice->CreateHeap ( &bufferHeapDesc, IID_PPV_ARGS ( &bufferHeap ) ) );


	// buffer heap with upload memory. Use for cpu write, gpu read buffer resources
	gfx_heap_desc_t uploadHeapDesc = CD3DX12_HEAP_DESC ( BUFFER_HEAP_SIZE,
														 D3D12_HEAP_TYPE_UPLOAD,
														 D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
														 D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS );

	DXERROR( gfxDevice->CreateHeap ( &uploadHeapDesc, IID_PPV_ARGS ( &uploadHeap ) ) );
	

	// texture heap with default memory.
	gfx_heap_desc_t textureHeapDesc = CD3DX12_HEAP_DESC ( TEXTURE_HEAP_SIZE,
														  D3D12_HEAP_TYPE_DEFAULT,
														  D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
														  D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES );

	DXERROR( gfxDevice->CreateHeap ( &textureHeapDesc, IID_PPV_ARGS ( &textureHeap ) ) );

	bufferHeapOffset = 0;
	uploadHeapOffset = 0;
	textureHeapOffset = 0;
}


GfxMemManager::~GfxMemManager ()
{
}

gfx_resource_t * GfxMemManager::allocateConstantBuffer ( uint size, gfx_resource_state_t state, uint flags )
{
	uint alignedSize = NEAREST_MULTIPLE ( size, 256 );
	bool isWriteCombine = !!( flags & GFX_MEM_WRITE_COMBINE );
	gfx_heap_t *heap = isWriteCombine ? uploadHeap : bufferHeap;
	uint *heapOffset = isWriteCombine ? &uploadHeapOffset : &bufferHeapOffset;
	gfx_resource_t *cbuffer = nullptr;

	gfx_resource_desc_t constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer ( alignedSize );

	DXERROR ( gfxDevice->CreatePlacedResource ( heap, *heapOffset, &constantBufferDesc, state, nullptr, IID_PPV_ARGS ( &cbuffer ) ) );

	assert ( cbuffer != nullptr );

	*heapOffset += alignedSize;
	staticResources[staticResourceCount++] = cbuffer;

	return cbuffer;
}
