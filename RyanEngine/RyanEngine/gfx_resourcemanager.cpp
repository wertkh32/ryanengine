#include "gfx_resourcemanager.h"



GfxResourceManager::GfxResourceManager ()
{
}


GfxResourceManager::~GfxResourceManager ()
{
}


void GfxResourceManager::Init( gfx_device_t *_gfxDevice )
{
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
	gfx_heap_desc_t uploadHeapDesc = CD3DX12_HEAP_DESC ( UPLOAD_HEAP_SIZE,
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


gfx_resource_t * GfxResourceManager::allocateBuffer ( const gfx_resource_desc_t *desc, uint resourceFlags )
{
	bool isUAV = !!( resourceFlags & GFX_RESOURCE_UAV );
	bool isWriteCombine = !!( resourceFlags & GFX_RESOURCE_WRITE_COMBINE );
	bool isGPUOnly = !!( resourceFlags & GFX_RESOURCE_GPU_ONLY );

	gfx_heap_t *heap = isWriteCombine ? uploadHeap : bufferHeap;
	uint *heapOffset = isWriteCombine ? &uploadHeapOffset : &bufferHeapOffset;

	gfx_resource_t *buffer = nullptr;
	gfx_resource_state_t state = D3D12_RESOURCE_STATE_COMMON;

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


bool GfxResourceManager::checkTextureResourceValid ( GFX_TEXTURE_FORMAT format, bool useMips )
{
	
	HRESULT hr;
	DXGI_FORMAT dxgiFormat = static_cast< DXGI_FORMAT >(format);

	D3D12_FEATURE_DATA_FORMAT_INFO formatInfo = { dxgiFormat };

	hr = gfxDevice->CheckFeatureSupport ( D3D12_FEATURE_FORMAT_INFO, &formatInfo, sizeof ( formatInfo ) );

	if ( hr != S_OK )
		return false; // format not supported

	if ( useMips ) // texture has mips
	{
		D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport;

		formatSupport.Format = dxgiFormat;
		formatSupport.Support1 = D3D12_FORMAT_SUPPORT1_MIP;

		hr = gfxDevice->CheckFeatureSupport ( D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport, sizeof ( formatSupport ) );

		if ( hr != S_OK )
			return false; // mips not supported
	}
	
	return true;
}


gfx_resource_t * GfxResourceManager::allocateTexture2D ( uint width, uint height, gfx_resource_state_t state, GFX_TEXTURE_FORMAT format, uint mipLevels )
{
	DXGI_FORMAT dxgiFormat = static_cast< DXGI_FORMAT >(format);
	gfx_resource_t *texture = nullptr;

	assert ( width <= MAX_TEXTURE_DIM && height <= MAX_TEXTURE_DIM );
#if USE( GFX_RUNTIME_CHECKS )
	assert ( checkTextureResourceValid ( format, ( mipLevels != 1 ) ) );
#endif

	gfx_resource_desc_t textureDesc = CD3DX12_RESOURCE_DESC::Tex2D ( dxgiFormat, width, height, 1, mipLevels );

	assert ( gfxDevice );

	DXERROR ( gfxDevice->CreatePlacedResource ( textureHeap, textureHeapOffset, &textureDesc, state, nullptr, IID_PPV_ARGS ( &texture ) ) );

	assert ( texture != nullptr );
	assert ( staticResourceCount < MAX_STATIC_RESOURCES );

	textureHeapOffset += getResourceAllocSize( &textureDesc );
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
