#include "gfx_gputexture.h"

GfxGpuTexture::GfxGpuTexture ( uint width, uint height, uint depth, GFX_TEXTURE_FORMAT texFormat, uint mipLevels, uint resourceFlags, GfxTextureType texType )
{
	GfxResourceManager *resman = GfxResourceManager::Instance ();
	DXGI_FORMAT dxgiFormat = static_cast< DXGI_FORMAT >(texFormat);
	bool isUAV = !!(resourceFlags & GFX_RESOURCE_UAV);
	
	gfx_resource_desc_t textureDesc;

#if USE( GFX_RUNTIME_CHECKS )
	checkTextureResourceValid ( texFormat, (mipLevels != 1) );
#endif

	assert ( width <= MAX_TEXTURE_DIM && height <= MAX_TEXTURE_DIM );

	type = texType;
	format = texFormat;

	switch ( type )
	{
		case GFX_TEXTURE_2D:
			textureDesc = CD3DX12_RESOURCE_DESC::Tex2D ( dxgiFormat, width, height, 1, mipLevels );
			break;
		case GFX_TEXTURE_2D_ARRAY:
			textureDesc = CD3DX12_RESOURCE_DESC::Tex2D ( dxgiFormat, width, height, depth, mipLevels );
			break;
		case GFX_TEXTURE_3D:
			textureDesc = CD3DX12_RESOURCE_DESC::Tex3D ( dxgiFormat, width, height, depth, mipLevels );
			break;
	}

	if ( isUAV )
		textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	resource = resman->allocateTexture ( &textureDesc, resourceFlags );
}

void GfxGpuTexture::CreateDefaultSRV ( gfx_desc_handle_t & descHandle )
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	D3D12_SRV_DIMENSION dimension = s_defaultSRVDimensions[type];
	DXGI_FORMAT dxgiFormat = static_cast< DXGI_FORMAT >(format);

	srvDesc.ViewDimension = dimension;
	
	switch ( type )
	{
	case GFX_TEXTURE_2D:
		srvDesc.Texture2D.MipLevels = -1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		break;
	case GFX_TEXTURE_2D_ARRAY:
		
		break;
	case GFX_TEXTURE_3D:
		
		break;
	}

	
}


bool GfxGpuTexture::checkTextureResourceValid ( GFX_TEXTURE_FORMAT format, bool useMips )
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
