#include "asset_manager.h"

typedef bool ( *AssetLoadFunc )( ASSET_TYPE type, const void *data );
typedef void ( *AssetAddFunc )( const void *asset );

static AssetLoadFunc s_assetLoadFunctions[] =
{
	nullptr,
	nullptr,
	nullptr,
};

sassert ( _countof ( s_assetLoadFunctions ) == ASSET_TYPE_COUNT );

static AssetAddFunc s_assetAddFunctions[] =
{
	nullptr,
	nullptr,
	nullptr,
};

sassert ( _countof ( s_assetAddFunctions ) == ASSET_TYPE_COUNT );


AssetManager::AssetManager ()
{
}


AssetManager::~AssetManager ()
{
}
