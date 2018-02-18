#include "gfx_gputexture.h"

GfxGpuTexture::GfxGpuTexture ( uint _width, uint _height, uint _depth, GFX_FORMAT _format, uint _mipLevels, uint _resourceFlags, GfxTextureType _type )
{
	GfxResourceManager *resman = GfxResourceManager::Instance ();
	DXGI_FORMAT dxgiFormat = static_cast< DXGI_FORMAT >( _format );
	bool isUAV = !!( _resourceFlags & GFX_RESOURCE_UAV );
	
	gfx_resource_desc_t textureDesc;

	assert ( width <= MAX_TEXTURE_DIM && height <= MAX_TEXTURE_DIM );

	type = _type;
	format = _format;
	width = _width;
	height = _height;
	depth = _depth;
	mipLevels = _mipLevels;
	isUAVAllowed = isUAV;

#if USE( GFX_RUNTIME_CHECKS )
	checkTextureResourceValid ( format, ( mipLevels != 1 ) );
#endif

	switch ( type )
	{
		case GFX_TEXTURE_2D:
		case GFX_TEXTURE_2D_CUBEMAP:
			textureDesc = CD3DX12_RESOURCE_DESC::Tex2D ( dxgiFormat, width, height, 1, mipLevels );
			break;
		case GFX_TEXTURE_2D_ARRAY:
		case GFX_TEXTURE_2D_CUBEMAP_ARRAY:
			textureDesc = CD3DX12_RESOURCE_DESC::Tex2D ( dxgiFormat, width, height, depth, mipLevels );
			break;
		case GFX_TEXTURE_3D:
			textureDesc = CD3DX12_RESOURCE_DESC::Tex3D ( dxgiFormat, width, height, depth, mipLevels );
			break;
	}

	if ( isUAV )
		textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	resource = resman->allocateTexture ( &textureDesc, _resourceFlags );
}

void GfxGpuTexture::CreateDefaultSRV ( gfx_desc_handle_t & descHandle )
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	DXGI_FORMAT dxgiFormat = static_cast< DXGI_FORMAT >(format);

	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	switch ( type )
	{
		case GFX_TEXTURE_2D:
			// all mips
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = -1;
			break;
		case GFX_TEXTURE_2D_ARRAY:
			// all mips, all depth
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.Texture2DArray.MipLevels = -1;
			srvDesc.Texture2DArray.ArraySize = depth;
			break;
		case GFX_TEXTURE_2D_CUBEMAP:
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.TextureCube.MipLevels = -1;
			break;
		case GFX_TEXTURE_2D_CUBEMAP_ARRAY:
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
			srvDesc.TextureCubeArray.MipLevels = -1;
			srvDesc.TextureCubeArray.NumCubes = depth / 6U;
			break;
		case GFX_TEXTURE_3D:
			// all mips
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
			srvDesc.Texture3D.MipLevels = -1;
			break;
	}

	g_gfxDevice->CreateShaderResourceView ( resource, &srvDesc, descHandle );
}

void GfxGpuTexture::CreateDefaultUAV ( gfx_desc_handle_t & descHandle )
{
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};

	DXGI_FORMAT dxgiFormat = static_cast< DXGI_FORMAT >(format);

	switch ( type )
	{
	case GFX_TEXTURE_2D:
	case GFX_TEXTURE_2D_CUBEMAP:
		// first mip
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		break;
	case GFX_TEXTURE_2D_ARRAY:
	case GFX_TEXTURE_2D_CUBEMAP_ARRAY:
		// first mip, all slices
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
		uavDesc.Texture2DArray.ArraySize = depth;
		break;
	case GFX_TEXTURE_3D:
		// first mip, all slices
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
		uavDesc.Texture3D.WSize = depth;
		break;
	}

	g_gfxDevice->CreateUnorderedAccessView ( resource, nullptr, &uavDesc, descHandle );
}

void GfxGpuTexture::CreateCustomTex2DSRV ( gfx_desc_handle_t & descHandle, uint firstMip, uint numMips, float minLODClamp )
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	DXGI_FORMAT dxgiFormat = static_cast< DXGI_FORMAT >(format);

	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = firstMip;
	srvDesc.Texture2D.MipLevels = numMips;
	srvDesc.Texture2D.ResourceMinLODClamp = minLODClamp;

	g_gfxDevice->CreateShaderResourceView ( resource, &srvDesc, descHandle );
}

void GfxGpuTexture::CreateCustomTex2DUAV ( gfx_desc_handle_t & descHandle, uint mipSlice )
{
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};

	DXGI_FORMAT dxgiFormat = static_cast< DXGI_FORMAT >(format);
	
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = mipSlice;

	g_gfxDevice->CreateUnorderedAccessView ( resource, nullptr, &uavDesc, descHandle );
}


bool GfxGpuTexture::checkTextureResourceValid ( GFX_FORMAT format, bool useMips )
{

	HRESULT hr;
	DXGI_FORMAT dxgiFormat = static_cast< DXGI_FORMAT >(format);

	D3D12_FEATURE_DATA_FORMAT_INFO formatInfo = { dxgiFormat };

	hr = g_gfxDevice->CheckFeatureSupport ( D3D12_FEATURE_FORMAT_INFO, &formatInfo, sizeof ( formatInfo ) );

	if ( hr != S_OK )
		return false; // format not supported

	if ( useMips ) // texture has mips
	{
		D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport;

		formatSupport.Format = dxgiFormat;
		formatSupport.Support1 = D3D12_FORMAT_SUPPORT1_MIP;

		hr = g_gfxDevice->CheckFeatureSupport ( D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport, sizeof ( formatSupport ) );

		if ( hr != S_OK )
			return false; // mips not supported
	}

	return true;
}



GfxGpuTexture::~GfxGpuTexture ()
{
}
