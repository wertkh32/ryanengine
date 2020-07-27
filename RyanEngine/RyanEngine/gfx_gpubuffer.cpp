#include "gfx_gpubuffer.h"


GfxGpuBuffer::GfxGpuBuffer ( uint _size, uint resourceFlags )
{
	GfxResourceManager *resman ;
	D3D12_RESOURCE_DESC desc;
	bool isWriteCombine;
	bool isUAV;

	resman = GfxResourceManager::Instance ();
	isWriteCombine = !!( resourceFlags & GFX_RESOURCE_WRITE_COMBINE );
	isUAV = !!( resourceFlags & GFX_RESOURCE_UAV );
	bufferSize = _size;
	isUAVAllowed = isUAV;

	desc = CD3DX12_RESOURCE_DESC::Buffer ( bufferSize, isUAV ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE, 0 );

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


void GfxGpuBuffer::CreateVertexBufferView ( D3D12_VERTEX_BUFFER_VIEW * outVBView, uint size, uint offset, uint stride )
{
	assert ( outVBView );
	assert ( offset + size <= bufferSize );

	outVBView->BufferLocation = resource->GetGPUVirtualAddress () + offset;
	outVBView->SizeInBytes = size;
	outVBView->StrideInBytes = stride;
}

void GfxGpuBuffer::CreateIndexBufferView ( D3D12_INDEX_BUFFER_VIEW * outIBView, uint size, uint offset, bool is16bits )
{
	assert ( outIBView );
	assert ( offset + size <= bufferSize );

	outIBView->BufferLocation = resource->GetGPUVirtualAddress () + offset;
	outIBView->Format = is16bits ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	outIBView->SizeInBytes = size;
}


void GfxGpuBuffer::CreateRawBufferView ( D3D12_CPU_DESCRIPTOR_HANDLE & descHandle, uint size, uint firstElement )
{
	assert ( g_gfxDevice );
	assert ( size <= bufferSize );

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_R32_TYPELESS; // required for raw buffers
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.NumElements = size / 4;
	srvDesc.Buffer.FirstElement = firstElement;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

	g_gfxDevice->CreateShaderResourceView ( resource, &srvDesc, descHandle );
}


void GfxGpuBuffer::CreateRawBufferUAV ( D3D12_CPU_DESCRIPTOR_HANDLE & descHandle, uint size, uint firstElement )
{
	assert ( g_gfxDevice );
	assert ( size <= bufferSize );
	assert ( isUAVAllowed );

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Format = DXGI_FORMAT_R32_TYPELESS; // required for raw buffers
	uavDesc.Buffer.NumElements = size / 4;
	uavDesc.Buffer.FirstElement = firstElement;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

	g_gfxDevice->CreateUnorderedAccessView ( resource, nullptr, &uavDesc, descHandle );
}


void GfxGpuBuffer::CreateStructuredBufferView ( D3D12_CPU_DESCRIPTOR_HANDLE & descHandle, uint sizeOfElement, uint numElements, uint firstElement )
{
	assert ( g_gfxDevice );
	assert ( sizeOfElement * numElements <= bufferSize );

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.NumElements = numElements;
	srvDesc.Buffer.StructureByteStride = sizeOfElement;
	srvDesc.Buffer.FirstElement = firstElement;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	g_gfxDevice->CreateShaderResourceView ( resource, &srvDesc, descHandle );
}


void GfxGpuBuffer::CreateTypedBufferView ( D3D12_CPU_DESCRIPTOR_HANDLE & descHandle, GFX_TBUFFER_FORMAT format, uint numElements, uint firstElement )
{
	uint elemSize;

	assert ( g_gfxDevice );
	
	elemSize = Gfx_GetTypedBufferByteSize ( format );
	
	assert ( elemSize != 0 );
	assert ( elemSize * numElements <= bufferSize );

	DXGI_FORMAT dxgiFormat = static_cast< DXGI_FORMAT >( format );

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = dxgiFormat;
	srvDesc.Buffer.NumElements = numElements;
	srvDesc.Buffer.FirstElement = firstElement;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	g_gfxDevice->CreateShaderResourceView ( resource, &srvDesc, descHandle );
}


void GfxGpuBuffer::CreateStructuredBufferUAV ( D3D12_CPU_DESCRIPTOR_HANDLE & descHandle, uint sizeOfElement, uint numElements, uint firstElement )
{
	assert ( g_gfxDevice );
	assert ( sizeOfElement * numElements <= bufferSize );
	assert ( isUAVAllowed );

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.CounterOffsetInBytes = 0;
	uavDesc.Buffer.NumElements = numElements;
	uavDesc.Buffer.StructureByteStride = sizeOfElement;
	uavDesc.Buffer.FirstElement = firstElement;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

	g_gfxDevice->CreateUnorderedAccessView ( resource, nullptr, &uavDesc, descHandle );
}


void GfxGpuBuffer::CreateStructuredBufferUAVWithCounter ( D3D12_CPU_DESCRIPTOR_HANDLE & descHandle, uint sizeOfElement, uint numElements, ID3D12Resource * counterBuffer, uint counterOffset, uint firstElement )
{
	assert ( g_gfxDevice );
	assert ( counterBuffer );
	assert ( sizeOfElement * numElements <= bufferSize );
	assert ( isUAVAllowed );

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.CounterOffsetInBytes = counterOffset;
	uavDesc.Buffer.NumElements = numElements;
	uavDesc.Buffer.StructureByteStride = sizeOfElement;
	uavDesc.Buffer.FirstElement = firstElement;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

	g_gfxDevice->CreateUnorderedAccessView ( resource, counterBuffer, &uavDesc, descHandle );
}


void GfxGpuBuffer::CreateTypedUAV ( D3D12_CPU_DESCRIPTOR_HANDLE & descHandle, GFX_TBUFFER_FORMAT format, uint numElements, uint firstElement )
{
	uint elemSize;

	assert ( g_gfxDevice );

	elemSize = Gfx_GetTypedBufferByteSize ( format );

	assert ( elemSize != 0 );
	assert ( elemSize * numElements <= bufferSize );

	DXGI_FORMAT dxgiFormat = static_cast< DXGI_FORMAT >(format);

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Format = dxgiFormat;
	uavDesc.Buffer.NumElements = numElements;
	uavDesc.Buffer.FirstElement = firstElement;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

	g_gfxDevice->CreateUnorderedAccessView ( resource, nullptr, &uavDesc, descHandle );
}


void GfxGpuBuffer::CreateConstantBufferView ( D3D12_CPU_DESCRIPTOR_HANDLE & descHandle, uint offset, uint size )
{
	assert ( g_gfxDevice );
	assert ( offset + size <= bufferSize );

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = resource->GetGPUVirtualAddress () + ALIGN_UP( offset, GFX_CONSTANT_BUFFER_ALIGNMENT );
	cbvDesc.SizeInBytes = ALIGN_UP( size, GFX_CONSTANT_BUFFER_ALIGNMENT );
	
	g_gfxDevice->CreateConstantBufferView ( &cbvDesc, descHandle );
}
