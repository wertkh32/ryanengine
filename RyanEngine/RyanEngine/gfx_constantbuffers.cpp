#include "gfx_constantbuffers.h"

void Gfx_CreateConstantBuffer( gfx_device_t *gfxDevice, gfx_cbuffer **cbuffer, gfx_desc_handle_t descHandle, uint size )
{
	uint alignedSize = NEAREST_MULTIPLE ( size, 256 );

	DXERROR( gfxDevice->CreateCommittedResource(
			 &CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD ), // D3D12_HEAP_TYPE_UPLOAD = write combine memory, D3D12_HEAP_TYPE_READBACK == system coherent, D3D12_HEAP_TYPE_DEFAULT == gpu only, no cpu access
			 D3D12_HEAP_FLAG_NONE,
			 &CD3DX12_RESOURCE_DESC::Buffer( alignedSize ),
			 D3D12_RESOURCE_STATE_GENERIC_READ,
			 nullptr,
			 IID_PPV_ARGS( cbuffer ) ) );

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = (*cbuffer)->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = alignedSize;

	gfxDevice->CreateConstantBufferView( &cbvDesc, descHandle );

}