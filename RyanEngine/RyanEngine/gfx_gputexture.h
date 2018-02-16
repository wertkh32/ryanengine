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
