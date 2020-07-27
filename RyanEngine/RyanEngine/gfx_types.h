#pragma once
#include "feature.h"

#if USE( DX12_API )
#include "gfx_dx12_types.h"
#endif

struct GfxVertBuffer
{
	ID3D12Resource *buffer;
	D3D12_VERTEX_BUFFER_VIEW bufferView;
	byte *mappedPtr;
};


struct GfxIndexBuffer
{
	ID3D12Resource *buffer;
	D3D12_INDEX_BUFFER_VIEW bufferView;
	byte *mappedPtr;
};


struct GfxBuffer
{
	ID3D12Resource *buffer;
	byte *mappedPtr;
};

typedef GfxBuffer GfxConstantBuffer;