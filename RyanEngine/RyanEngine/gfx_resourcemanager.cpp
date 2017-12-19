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


gfx_resource_t * GfxResourceManager::allocateBuffer ( const gfx_resource_desc_t *desc, gfx_resource_state_t state, uint memFlags )
{
	bool isWriteCombine = !!(memFlags & GFX_RESOURCE_WRITE_COMBINE);
	gfx_heap_t *heap = isWriteCombine ? uploadHeap : bufferHeap;
	uint *heapOffset = isWriteCombine ? &uploadHeapOffset : &bufferHeapOffset;
	gfx_resource_t *buffer = nullptr;

	assert ( gfxDevice );

	DXERROR ( gfxDevice->CreatePlacedResource ( heap, *heapOffset, desc, state, nullptr, IID_PPV_ARGS ( &buffer ) ) );

	assert ( buffer != nullptr );
	assert ( staticResourceCount < MAX_STATIC_RESOURCES );

	*heapOffset += getResourceAllocSize ( desc );
	staticResources[staticResourceCount++] = buffer;

	return buffer;
}


void GfxResourceManager::createConstantBuffer ( uint size, GfxConstantBuffer * constantBuffer, gfx_desc_handle_t descHandle )
{
	gfx_resource_t *cbuf_resource;
	byte *dataPtr;
	uint alignedSize = NEAREST_MULTIPLE ( size, 256 ); // constant buffers have to be aligned to 256

	gfx_resource_desc_t bufferDesc = CD3DX12_RESOURCE_DESC::Buffer ( alignedSize );

	cbuf_resource = allocateBuffer ( &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, GFX_RESOURCE_WRITE_COMBINE );

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = cbuf_resource->GetGPUVirtualAddress ();
	cbvDesc.SizeInBytes = alignedSize;

	gfxDevice->CreateConstantBufferView ( &cbvDesc, descHandle );

	
	CD3DX12_RANGE readRange ( 0, 0 ); // ( begin <= end ) means not read by the cpu 
	cbuf_resource->Map ( 0, &readRange, reinterpret_cast< void** >(&dataPtr) );
	// use persistent map here, since heap type is upload only. Make sure cpu updates this before execute command lists is called.
	// pointer is to write-combine memory. Ensure cpu does not read this, not even accidentally.

	constantBuffer->buffer = cbuf_resource;
	constantBuffer->mappedPtr = dataPtr;
}


void GfxResourceManager::createRawBuffer ( uint size, GfxBuffer *buffer, uint resourceFlags, gfx_desc_handle_t descHandle )
{
	gfx_resource_t *resource;
	byte *dataPtr = nullptr;
	uint accessFlags = 0;
	gfx_resource_state_t resourceState = D3D12_RESOURCE_STATE_COMMON;

	bool isUAV = !!(resourceFlags & GFX_RESOURCE_UAV);
	bool isWriteCombine = !!(resourceFlags & GFX_RESOURCE_WRITE_COMBINE);
	bool isGPUOnly = !!(resourceFlags & GFX_RESOURCE_GPU_ONLY);

	assert ( resourceFlags );
	assert ( !isWriteCombine || !isGPUOnly );

	gfx_resource_desc_t bufferDesc = CD3DX12_RESOURCE_DESC::Buffer ( size );

	if ( isUAV )
	{
		bufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		resourceState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	}

	if ( isWriteCombine )
	{
		accessFlags = GFX_RESOURCE_WRITE_COMBINE;
		resourceState = D3D12_RESOURCE_STATE_GENERIC_READ; // required for upload heaps // must override uav
	}
	else
		accessFlags = GFX_RESOURCE_GPU_ONLY;

	resource = allocateBuffer ( &bufferDesc, resourceState, accessFlags );

	if( isUAV )
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};

		uavDesc.Format = DXGI_FORMAT_R32_TYPELESS; // required for raw buffers
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.NumElements = size / 4;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

		gfxDevice->CreateUnorderedAccessView ( resource, nullptr, &uavDesc, descHandle );
	}
	else
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Format = DXGI_FORMAT_R32_TYPELESS; // required for raw buffers
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Buffer.NumElements = size / 4;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

		gfxDevice->CreateShaderResourceView ( resource, &srvDesc, descHandle );
	}
	
	if ( isWriteCombine )
	{
		CD3DX12_RANGE readRange ( 0, 0 ); // ( begin <= end ) means not read by the cpu 
		resource->Map ( 0, &readRange, reinterpret_cast< void** >(&dataPtr) );
		// use persistent map here, since heap type is upload only. Make sure cpu updates this before execute command lists is called.
		// pointer is to write-combine memory. Ensure cpu does not read this, not even accidentally.
	}

	buffer->buffer = resource;
	buffer->mappedPtr = dataPtr;
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

	gfx_resource_desc_t textureDesc = CD3DX12_RESOURCE_DESC::Tex2D ( dxgiFormat, width, width, 1, mipLevels );

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
