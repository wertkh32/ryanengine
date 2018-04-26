
#include <stdio.h>
#include <stdlib.h>
#include "AssetFile.h"

int main()
{
	printf ( "Asset Builder: Transforms raw assets into game engine readable assets" );

	{
		const char *dataStuff = "123456789";
		AssetFileWriter writer;
		writer.beginWriteAssetFile ( "test.data" );
		writer.addAsset ( ASSET_TYPE_MODEL, dataStuff, 10 );
		writer.endWriteAssetFile ();
	}


	{
		char buffer[256];
		AssetFileReader reader;
		ASSET_TYPE type;
		uint32_t size;
		reader.beginReadAssetFile ( "test.data" );
		reader.getNextAsset ( &type, buffer, 256, &size );
		reader.endReadAssetFile ();

		printf ( "type %d, size %d, data %s", type, size, buffer );
	}


	system ( "pause" );
    return 0;
}

