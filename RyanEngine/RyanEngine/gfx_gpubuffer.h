#pragma once
#include "universal.h"
#include "gfx_globals.h"
#include "gfx_resourcemanager.h"

#define GPU_BUFFER_MIN_SIZE	256

class GfxGpuBuffer
{
private:
public:
	gfx_resource_t *resource;
	byte *mappedPtr;

	GfxGpuBuffer ( uint size, uint resourceFlags );
	~GfxGpuBuffer ();

	// non-SRVs/UAVs
	void CreateVertexBufferView ();
	void CreateIndexBufferView ();

	// CBV/SRV/UAVs
	void CreateRawBufferView ();
	void CreateStructuredBufferView ();
	void CreateConstantBufferView (); // address needs to be (the reads need to be ) 256 byte aligned

};
