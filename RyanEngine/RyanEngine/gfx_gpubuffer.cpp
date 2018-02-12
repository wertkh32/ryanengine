#include "gfx_gpubuffer.h"


GfxGpuBuffer::GfxGpuBuffer ( uint _size, uint resourceFlags )
{
	GfxResourceManager *resman ;
	gfx_resource_desc_t desc;
	bool isWriteCombine;

	resman = GfxResourceManager::Instance ();
	isWriteCombine = !!(resourceFlags & GFX_RESOURCE_WRITE_COMBINE);
	bufferSize = _size;

	desc = CD3DX12_RESOURCE_DESC::Buffer ( bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, 0 );

	resource = resman->allocateBuffer ( &desc, resourceFlags );

	if ( isWriteCombine )
	{
		CD3DX12_RANGE readRange ( 0, 0 ); // ( begin <= end ) means not read by the cpu 
		resource->Map ( 0, &readRange, reinterpret_cast< void** >(&mappedPtr) );
		// use persistent map here, since heap type is upload only. Make sure cpu updates this before execute command lists is called.
		// pointer is to write-combine memory. Ensure cpu does not read this, not even accidentally.
	}

}


GfxGpuBuffer::~GfxGpuBuffer ()
{
}


void GfxGpuBuffer::CreateVertexBufferView ( gfx_vert_buffer_view_t * outVBView, uint size, uint offset, uint stride )
{
	assert ( outVBView );
	assert ( offset + size <= bufferSize );

	outVBView->BufferLocation = resource->GetGPUVirtualAddress () + offset;
	outVBView->SizeInBytes = size;
	outVBView->StrideInBytes = stride;
}

void GfxGpuBuffer::CreateIndexBufferView ( gfx_index_buffer_view_t * outIBView, uint size, uint offset, bool is16bits )
{
	assert ( outIBView );
	assert ( offset + size <= bufferSize );

	outIBView->BufferLocation = resource->GetGPUVirtualAddress () + offset;
	outIBView->Format = is16bits ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	outIBView->SizeInBytes = size;
}


void GfxGpuBuffer::CreateRawBufferView ( gfx_desc_handle_t & descHandle, uint size )
{
	assert ( g_gfxDevice );
	assert ( size <= bufferSize );

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_R32_TYPELESS; // required for raw buffers
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.NumElements = size / 4;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

	g_gfxDevice->CreateShaderResourceView ( resource, &srvDesc, descHandle );
}


void GfxGpuBuffer::CreateRawBufferUAV ( gfx_desc_handle_t & descHandle, uint size )
{
	assert ( g_gfxDevice );
	assert ( size <= bufferSize );

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Format = DXGI_FORMAT_R32_TYPELESS; // required for raw buffers
	uavDesc.Buffer.NumElements = size / 4;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

	g_gfxDevice->CreateUnorderedAccessView ( resource, nullptr, &uavDesc, descHandle );
}


void GfxGpuBuffer::CreateStructuredBufferView ( gfx_desc_handle_t & descHandle, uint sizeOfElement, uint numElements )
{
	assert ( g_gfxDevice );
	assert ( sizeOfElement * numElements <= bufferSize );

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.NumElements = numElements;
	srvDesc.Buffer.StructureByteStride = sizeOfElement;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	g_gfxDevice->CreateShaderResourceView ( resource, &srvDesc, descHandle );
}


void GfxGpuBuffer::CreateStructuredBufferUAV ( gfx_desc_handle_t & descHandle, uint sizeOfElement, uint numElements )
{
	assert ( g_gfxDevice );
	assert ( sizeOfElement * numElements <= bufferSize );

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.CounterOffsetInBytes = 0;
	uavDesc.Buffer.NumElements = numElements;
	uavDesc.Buffer.StructureByteStride = sizeOfElement;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

	g_gfxDevice->CreateUnorderedAccessView ( resource, nullptr, &uavDesc, descHandle );
}

void GfxGpuBuffer::CreateStructuredBufferUAVWithCounter ( gfx_desc_handle_t & descHandle, uint sizeOfElement, uint numElements, gfx_resource_t * counterBuffer, uint counterOffset )
{
	assert ( g_gfxDevice );
	assert ( counterBuffer );
	assert ( sizeOfElement * numElements <= bufferSize );

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.CounterOffsetInBytes = counterOffset;
	uavDesc.Buffer.NumElements = numElements;
	uavDesc.Buffer.StructureByteStride = sizeOfElement;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

	g_gfxDevice->CreateUnorderedAccessView ( resource, counterBuffer, &uavDesc, descHandle );
}


void GfxGpuBuffer::CreateConstantBufferView ( gfx_desc_handle_t & descHandle, uint offset, uint size )
{
	assert ( g_gfxDevice );
	assert ( offset + size <= bufferSize );

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = resource->GetGPUVirtualAddress () + ALIGN_UP( offset, GFX_CONSTANT_BUFFER_ALIGNMENT );
	cbvDesc.SizeInBytes = ALIGN_UP( size, GFX_CONSTANT_BUFFER_ALIGNMENT );
	
	g_gfxDevice->CreateConstantBufferView ( &cbvDesc, descHandle );
}
