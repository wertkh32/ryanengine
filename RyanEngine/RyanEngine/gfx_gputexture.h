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
	GFX_TEXTURE_FORMAT format;

	GfxGpuTexture ( uint width, uint height, uint depth, GFX_TEXTURE_FORMAT texFormat, uint mipLevels, uint resourceFlags, GfxTextureType texType );

	void CreateDefaultSRV ( gfx_desc_handle_t& descHandle );
	void CreateDefaultUAV ( gfx_desc_handle_t& descHandle, uint mipLevel );


	bool checkTextureResourceValid ( GFX_TEXTURE_FORMAT format, bool useMips );
	~GfxGpuTexture ();
};
