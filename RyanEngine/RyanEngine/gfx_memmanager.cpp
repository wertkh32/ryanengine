#include "gfx_memmanager.h"



GfxMemManager::GfxMemManager ()
{
}


GfxMemManager::~GfxMemManager ()
{
}


void GfxMemManager::Init( gfx_device_t *_gfxDevice )
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


gfx_resource_t * GfxMemManager::allocateConstantBuffer ( uint size, gfx_resource_state_t state, uint memFlags )
{
	uint alignedSize = NEAREST_MULTIPLE ( size, 256 );
	bool isWriteCombine = !!( memFlags & GFX_MEM_WRITE_COMBINE );
	gfx_heap_t *heap = isWriteCombine ? uploadHeap : bufferHeap;
	uint *heapOffset = isWriteCombine ? &uploadHeapOffset : &bufferHeapOffset;
	gfx_resource_t *cbuffer = nullptr;

	gfx_resource_desc_t constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer ( alignedSize );

	assert ( gfxDevice );

	DXERROR ( gfxDevice->CreatePlacedResource ( heap, *heapOffset, &constantBufferDesc, state, nullptr, IID_PPV_ARGS ( &cbuffer ) ) );

	assert ( cbuffer != nullptr );

	*heapOffset += getResourceAllocSize ( &constantBufferDesc );
	staticResources[staticResourceCount++] = cbuffer;

	return cbuffer;
}


bool GfxMemManager::checkTextureResourceValid ( GFX_TEXTURE_FORMAT format, bool useMips )
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


gfx_resource_t * GfxMemManager::allocateTexture2D ( uint width, uint height, gfx_resource_state_t state, GFX_RESOURCE_ACCESS access, GFX_TEXTURE_FORMAT format, uint mipLevels )
{
	bool isRW = ( access == GFX_RESOURCE_READ_WRITE );
	DXGI_FORMAT dxgiFormat = static_cast< DXGI_FORMAT >(format);
	gfx_resource_t *texture = nullptr;

	assert ( width <= MAX_TEXTURE_DIM && height <= MAX_TEXTURE_DIM );
#if USE( GFX_RUNTIME_CHECKS )
	assert ( checkTextureResourceValid ( format, ( mipLevels != 1 ) ) );
#endif

	gfx_resource_desc_t textureDesc = {};
	textureDesc.MipLevels = mipLevels;
	textureDesc.Format = dxgiFormat;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	textureDesc.DepthOrArraySize = 1;
	textureDesc.SampleDesc.Count = 1; // dont change this or the texture alignment will become 4MB
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	DXERROR ( gfxDevice->CreatePlacedResource ( textureHeap, textureHeapOffset, &textureDesc, state, nullptr, IID_PPV_ARGS ( &texture ) ) );

	assert ( texture != nullptr );

	textureHeapOffset += getResourceAllocSize( &textureDesc );
	staticResources[staticResourceCount++] = texture;

	return texture;
}
