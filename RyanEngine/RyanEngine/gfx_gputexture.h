#pragma once
#include "universal.h"
#include "gfx_globals.h"
#include "gfx_resourcemanager.h"

enum GfxTextureType
{
	GFX_TEXTURE_2D,
	GFX_TEXTURE_2D_ARRAY,
	GFX_TEXTURE_2D_CUBEMAP,
	GFX_TEXTURE_2D_CUBEMAP_ARRAY,
	GFX_TEXTURE_3D,

	GFX_TEXTURE_TYPE_COUNT
};


struct GfxGpuTexture
{
	ID3D12Resource *resource;
	GfxTextureType type;
	GFX_FORMAT format;
	uint width;
	uint height;
	uint depth;
	uint mipLevels;
	bool isUAVAllowed;

	GfxGpuTexture ( uint _width, uint _height, uint _depth, GFX_FORMAT _format, uint _mipLevels, uint _resourceFlags, GfxTextureType _type );

	void CreateDefaultSRV ( D3D12_CPU_DESCRIPTOR_HANDLE& descHandle );
	void CreateDefaultUAV ( D3D12_CPU_DESCRIPTOR_HANDLE& descHandle );

	void CreateCustomTex2DSRV ( D3D12_CPU_DESCRIPTOR_HANDLE& descHandle, uint firstMip, uint numMips, float minLODClamp );
	void CreateCustomTex2DUAV ( D3D12_CPU_DESCRIPTOR_HANDLE& descHandle, uint mipSlice );


	bool checkTextureResourceValid ( GFX_FORMAT format, bool useMips );
	~GfxGpuTexture ();
};
