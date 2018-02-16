#pragma once
#include "universal.h"
#include "gfx_globals.h"
#include "gfx_resourcemanager.h"

enum GfxTextureType
{
	GFX_TEXTURE_2D,
	GFX_TEXTURE_2D_ARRAY,
	GFX_TEXTURE_3D,

	GFX_TEXTURE_TYPE_COUNT
};


D3D12_SRV_DIMENSION s_defaultSRVDimensions[GFX_TEXTURE_TYPE_COUNT] =
{
	D3D12_SRV_DIMENSION_TEXTURE2D,
	D3D12_SRV_DIMENSION_TEXTURE2DARRAY,
	D3D12_SRV_DIMENSION_TEXTURE3D
};



D3D12_UAV_DIMENSION s_defaultUAVDimensions[GFX_TEXTURE_TYPE_COUNT] =
{
	D3D12_UAV_DIMENSION_TEXTURE2D,
	D3D12_UAV_DIMENSION_TEXTURE2DARRAY,
	D3D12_UAV_DIMENSION_TEXTURE3D
};


struct GfxGpuTexture
{
	gfx_resource_t *resource;
	GfxTextureType type;
	GFX_FORMAT format;
	uint width;
	uint height;
	uint depth;
	uint mipLevels;
	bool isUAVAllowed;

	GfxGpuTexture ( uint _width, uint _height, uint _depth, GFX_FORMAT _format, uint _mipLevels, uint _resourceFlags, GfxTextureType _type );

	void CreateDefaultSRV ( gfx_desc_handle_t& descHandle );
	void CreateDefaultUAV ( gfx_desc_handle_t& descHandle );

	void CreateCustomTex2DSRV ( gfx_desc_handle_t& descHandle, uint firstMip, uint numMips );
	void CreateCustomTex2DUAV ( gfx_desc_handle_t& descHandle, uint mipSlice );


	bool checkTextureResourceValid ( GFX_FORMAT format, bool useMips );
	~GfxGpuTexture ();
};
