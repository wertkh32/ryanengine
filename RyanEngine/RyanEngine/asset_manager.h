#pragma once
#include "universal.h"
#include "asset_structs.h"

enum ASSET_TYPE
{
	ASSET_TYPE_MODEL,
	ASSET_TYPE_SHADER,
	ASSET_TYPE_TEXTURE,

	ASSET_TYPE_COUNT,
};

enum ASSET_POOL_SIZE : uint
{
	ASSET_POOL_MODEL_SIZE = 256,
	ASSET_POOL_SHADER_SIZE = 256,
	ASSET_POOL_TEXTURE_SIZE = 256,
};


const uint g_assetPoolSize[] =
{
	ASSET_POOL_MODEL_SIZE,		//ASSET_TYPE_MODEL
	ASSET_POOL_SHADER_SIZE,		//ASSET_TYPE_SHADER
	ASSET_POOL_TEXTURE_SIZE,	//ASSET_TYPE_TEXTURE
};


struct AssetHeader
{
	ASSET_TYPE type;
	uint dataSize;
};


template< typename T >
struct Asset
{
	AssetHeader header;
	T asset;
};


template< typename T, uint POOL_SIZE >
struct AssetPool
{
	Asset<T> pool[POOL_SIZE];

	const uint poolTotalSize = POOL_SIZE;
	uint poolCurrentSize;

	AssetPool ()
	{
		poolCurrentSize = 0;
	}

	bool addAsset ( const Asset<T> *asset )
	{
		if ( poolCurrentSize < POOL_SIZE );
		{
			pool[poolCurrentSize++] = *asset;
			return true;
		}

		return false;
	}

	void clearAssets ()
	{
		poolCurrentSize = 0;
	}

	uint getAssetIndex ( const Asset<T>* asset )
	{
		int index;

		index = asset - pool;

		assert ( index >= 0 && index < POOL_SIZE );

		return index;
	}
};


class AssetManager : public Singleton<AssetManager>
{
	friend Singleton<AssetManager>;
	AssetManager ();
	~AssetManager ();
public:
	void allocateAsset ( ASSET_TYPE type );
};

