#pragma once
#include "universal.h"
#include "gfx_globals.h"
#include "gfx_resourcemanager.h"


struct GfxGpuBuffer
{
	ID3D12Resource *resource;
	byte *mappedPtr;
	uint bufferSize;
	bool isUAVAllowed;


	GfxGpuBuffer ( uint size, uint resourceFlags );
	~GfxGpuBuffer ();

	// non-SRVs/UAVs
	void CreateVertexBufferView ( D3D12_VERTEX_BUFFER_VIEW *outVBView , uint size, uint stride, uint offset );
	void CreateIndexBufferView ( D3D12_INDEX_BUFFER_VIEW *outIBView, uint size, uint offset, bool is16bits = true ); // do we really need more that 65535 verts for a model?

	// CBV/SRV/UAVs
	void CreateRawBufferView ( D3D12_CPU_DESCRIPTOR_HANDLE &descHandle, uint size, uint firstElement = 0  );
	void CreateStructuredBufferView ( D3D12_CPU_DESCRIPTOR_HANDLE &descHandle, uint sizeOfElement, uint numElements, uint firstElement = 0 );
	void CreateTypedBufferView ( D3D12_CPU_DESCRIPTOR_HANDLE &descHandle, GFX_TBUFFER_FORMAT format, uint numElements, uint firstElement = 0 );

	void CreateRawBufferUAV ( D3D12_CPU_DESCRIPTOR_HANDLE &descHandle, uint size, uint firstElement = 0 );
	void CreateStructuredBufferUAV ( D3D12_CPU_DESCRIPTOR_HANDLE &descHandle, uint sizeOfElement, uint numElements, uint firstElement = 0 );
	void CreateStructuredBufferUAVWithCounter ( D3D12_CPU_DESCRIPTOR_HANDLE &descHandle, uint sizeOfElement, uint numElements, ID3D12Resource *counterBuffer, uint counterOffset, uint firstElement = 0 );
	void CreateTypedUAV ( D3D12_CPU_DESCRIPTOR_HANDLE &descHandle, GFX_TBUFFER_FORMAT format, uint numElements, uint firstElement = 0 );

	void CreateConstantBufferView ( D3D12_CPU_DESCRIPTOR_HANDLE &descHandle, uint offset, uint size ); // address needs to be (the reads need to be ) 256 byte aligned

};
