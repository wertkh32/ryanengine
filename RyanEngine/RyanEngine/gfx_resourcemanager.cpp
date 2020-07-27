#include "gfx_resourcemanager.h"



GfxResourceManager::GfxResourceManager ()
{
}


GfxResourceManager::~GfxResourceManager ()
{
}


void GfxResourceManager::Init( ID3D12Device *_gfxDevice )
{
	staticResourceCount = 0;
	dynamicResourceCount = 0;

	gfxDevice = _gfxDevice;

	for ( uint i = 0; i < MAX_STATIC_RESOURCES; i++ )
		staticResources[i] = NULL;

	for ( uint i = 0; i < MAX_DYNAMIC_RESOURCES; i++ )
		dynamicResources[i] = NULL;


	// buffer heap with default memory
	D3D12_HEAP_DESC bufferHeapDesc = CD3DX12_HEAP_DESC ( BUFFER_HEAP_SIZE,
														D3D12_HEAP_TYPE_DEFAULT, 
														D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, 
														D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS );

	DXERROR( gfxDevice->CreateHeap ( &bufferHeapDesc, IID_PPV_ARGS ( &bufferHeap ) ) );


	// buffer heap with upload memory. Use for cpu write, gpu read buffer resources
	D3D12_HEAP_DESC uploadHeapDesc = CD3DX12_HEAP_DESC ( UPLOAD_HEAP_SIZE,
														 D3D12_HEAP_TYPE_UPLOAD,
														 D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
														 D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS );

	DXERROR( gfxDevice->CreateHeap ( &uploadHeapDesc, IID_PPV_ARGS ( &uploadHeap ) ) );
	

	// texture heap with default memory.
	D3D12_HEAP_DESC textureHeapDesc = CD3DX12_HEAP_DESC ( TEXTURE_HEAP_SIZE,
														  D3D12_HEAP_TYPE_DEFAULT,
														  D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
														  D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES );

	DXERROR( gfxDevice->CreateHeap ( &textureHeapDesc, IID_PPV_ARGS ( &textureHeap ) ) );

	bufferHeapOffset = 0;
	uploadHeapOffset = 0;
	textureHeapOffset = 0;
}


ID3D12Resource * GfxResourceManager::allocateBuffer ( const D3D12_RESOURCE_DESC *desc, uint resourceFlags )
{
	bool isUAV = !!( resourceFlags & GFX_RESOURCE_UAV );
	bool isWriteCombine = !!( resourceFlags & GFX_RESOURCE_WRITE_COMBINE );

	ID3D12Heap *heap = isWriteCombine ? uploadHeap : bufferHeap;
	uint *heapOffset = isWriteCombine ? &uploadHeapOffset : &bufferHeapOffset;

	ID3D12Resource *buffer = nullptr;
	D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

	assert ( gfxDevice );

	if ( isWriteCombine )
		state = D3D12_RESOURCE_STATE_GENERIC_READ; // required for upload heaps // must override uav
	else if ( isUAV )
		state = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

	DXERROR ( gfxDevice->CreatePlacedResource ( heap, *heapOffset, desc, state, nullptr, IID_PPV_ARGS ( &buffer ) ) );

	assert ( buffer != nullptr );
	assert ( staticResourceCount < MAX_STATIC_RESOURCES );

	*heapOffset += getResourceAllocSize ( desc );
	staticResources[staticResourceCount++] = buffer;

	return buffer;
}


ID3D12Resource * GfxResourceManager::allocateTexture ( const D3D12_RESOURCE_DESC *textureDesc, uint resourceFlags )
{
	ID3D12Resource *texture = nullptr;
	bool isUAV = !!( resourceFlags & GFX_RESOURCE_UAV );
	D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COPY_DEST;

	assert ( gfxDevice );

	if ( isUAV )
		state = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

	DXERROR ( gfxDevice->CreatePlacedResource ( textureHeap, textureHeapOffset, textureDesc, state, nullptr, IID_PPV_ARGS ( &texture ) ) );

	assert ( texture != nullptr );
	assert ( staticResourceCount < MAX_STATIC_RESOURCES );

	textureHeapOffset += getResourceAllocSize( textureDesc );
	staticResources[staticResourceCount++] = texture;

	return texture;
}


void GfxResourceManager::deallocateAllDynamicResources ()
{
	// not implemented yet
}

void GfxResourceManager::deallocateAllStaticResources ()
{
	for ( uint i = 0;i < staticResourceCount; i++ )
	{
		staticResources[i]->Release ();
	}
}
