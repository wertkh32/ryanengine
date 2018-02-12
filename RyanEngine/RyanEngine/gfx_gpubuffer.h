#pragma once
#include "universal.h"
#include "gfx_globals.h"
#include "gfx_resourcemanager.h"


struct GfxGpuBuffer
{
	gfx_resource_t *resource;
	byte *mappedPtr;
	uint bufferSize;

	GfxGpuBuffer ( uint size, uint resourceFlags );
	~GfxGpuBuffer ();

	// non-SRVs/UAVs
	void CreateVertexBufferView ( gfx_vert_buffer_view_t *outVBView , uint size, uint stride, uint offset );
	void CreateIndexBufferView ( gfx_index_buffer_view_t *outIBView, uint size, uint offset, bool is16bits = true ); // do we really need more that 65535 verts for a model?

	// CBV/SRV/UAVs
	void CreateRawBufferView ( gfx_desc_handle_t &descHandle, uint size  );
	void CreateStructuredBufferView ( gfx_desc_handle_t &descHandle, uint sizeOfElement, uint numElements );

	void CreateRawBufferUAV ( gfx_desc_handle_t &descHandle, uint size );
	void CreateStructuredBufferUAV ( gfx_desc_handle_t &descHandle, uint sizeOfElement, uint numElements );
	void CreateStructuredBufferUAVWithCounter ( gfx_desc_handle_t &descHandle, uint sizeOfElement, uint numElements, gfx_resource_t *counterBuffer, uint counterOffset );
	
	void CreateConstantBufferView ( gfx_desc_handle_t &descHandle, uint offset, uint size ); // address needs to be (the reads need to be ) 256 byte aligned

};
