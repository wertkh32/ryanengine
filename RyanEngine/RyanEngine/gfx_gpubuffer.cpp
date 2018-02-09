#include "gfx_gpubuffer.h"


GfxGpuBuffer::GfxGpuBuffer ( uint size, uint resourceFlags )
{
	GfxResourceManager *resman ;
	gfx_resource_desc_t desc;
	bool isWriteCombine;

	resman = GfxResourceManager::Instance ();
	isWriteCombine = !!(resourceFlags & GFX_RESOURCE_WRITE_COMBINE);


	if ( size < GPU_BUFFER_MIN_SIZE )
		size = GPU_BUFFER_MIN_SIZE;

	desc = CD3DX12_RESOURCE_DESC::Buffer ( size );

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
