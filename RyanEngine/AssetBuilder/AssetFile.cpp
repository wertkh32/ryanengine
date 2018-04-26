#include "AssetFile.h"

uint32_t djb2_hash ( uint8_t *data, uint32_t size ) // from http://www.cse.yorku.ca/~oz/hash.html
{
	uint32_t hash = 5381;
	int c;
	uint32_t i = 0;

	while ( i != size )
	{
		c = data[i];
		i++;
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}

	return hash;
}


// Asset File Writer ///////////////////////////////////////////////////
AssetFileWriter::AssetFileWriter ()
{
	beginWrite = false;
}


AssetFileWriter::~AssetFileWriter ()
{
}


void AssetFileWriter::updateCurPos ( uint32_t inDataSize )
{
	uint32_t  prevCurPos;
	prevCurPos = curPos;

	curPos = ftell ( handle );
	assert ( ( curPos - prevCurPos ) == inDataSize );
	assert ( curPos == header.assetFileSize );
}


void AssetFileWriter::writeToCurrentFile ( const void * data, uint32_t inDataSize )
{
	fwrite ( data, 1, inDataSize, handle ); // write first to reserve header space
	header.assetFileSize += inDataSize;
	updateCurPos ( inDataSize );
}


bool AssetFileWriter::beginWriteAssetFile ( const char * filePath )
{
	handle = fopen ( filePath, "wb+" );

	if( handle == NULL )
		return false;

	memset ( &header, 0, sizeof ( AssetFileHeader ) );

	header.magic = 0;
	header.assetFileSize = 0;
	header.assetCount = 0;

	curPos = 0;
	beginWrite = true;

	writeToCurrentFile ( &header, sizeof ( header ) ); // write first to reserve header space

	return true;
}


bool AssetFileWriter::addAsset ( ASSET_TYPE type, const void * data, uint32_t inDataSize )
{
	if ( !inDataSize || data == nullptr )
		return false;

	assert ( handle );

	AssetHeader assetHeader;

	assetHeader.size = sizeof ( assetHeader ) + inDataSize;
	assetHeader.assetType = type;
	assetHeader.dataSize = inDataSize;

	writeToCurrentFile ( &assetHeader, sizeof ( assetHeader ) );

	writeToCurrentFile ( data, inDataSize );

	header.assetCount++;

	return true;
}


bool AssetFileWriter::endWriteAssetFile ()
{
	// calculate checksum
	assert ( handle );

	assert ( curPos == header.assetFileSize );

	header.magic = ASSET_FILE_MAGIC;

	fseek ( handle, 0, SEEK_SET );

	fwrite ( &header, 1, sizeof ( header ), handle ); // update header

	fflush ( handle );

	fclose ( handle );

	beginWrite = false;

	return false;
}
// end Asset File Writer ///////////////////////////////////////////////


// Asset File Reader ///////////////////////////////////////////////////
AssetFileReader::AssetFileReader ()
{
}

AssetFileReader::~AssetFileReader ()
{
}

void AssetFileReader::updateCurPos ( uint32_t inDataSize )
{
	uint32_t  prevCurPos;
	prevCurPos = curPos;

	curPos = ftell ( handle );
	assert ( (curPos - prevCurPos) == inDataSize );
}

void AssetFileReader::readToBuffer ( void * data, uint32_t inDataSize )
{
	fread ( data, 1, inDataSize, handle );
	updateCurPos ( inDataSize );
}

bool AssetFileReader::beginReadAssetFile ( const char * filePath )
{
	handle = fopen ( filePath, "rb" );

	if ( handle == NULL )
		return false;

	uint fileTotalSize;

	fseek ( handle, 0, SEEK_END );

	fileTotalSize = ftell ( handle );

	fseek ( handle, 0, SEEK_SET );

	curPos = 0;

	readToBuffer ( &header, sizeof ( AssetFileHeader ) );

	if ( header.assetFileSize != fileTotalSize )
	{
		fclose ( handle );
		return false;
	}

	totalBytes = fileTotalSize;

	return true;
}

bool AssetFileReader::getNextAsset ( ASSET_TYPE *outType, void *outData, uint32_t outDataBufferSize, uint32_t *outDataSize )
{
	AssetHeader assetHeader;

	uint startCurPos = curPos;

	readToBuffer ( &assetHeader, sizeof ( AssetHeader ) );

	uint32_t totalAssetSize = assetHeader.size;
	uint32_t dataSize = assetHeader.dataSize;
	ASSET_TYPE type = static_cast<ASSET_TYPE>(assetHeader.assetType);

	assert ( totalAssetSize == dataSize + sizeof ( AssetHeader ) );

	if ( dataSize > outDataBufferSize )
	{
		printf ( "Error: Buffer provided too small! outBufferDataSize:%u, dataSize:%u", outDataBufferSize, dataSize );
		return false;
	}

	*outType = type;
	*outDataSize = dataSize;

	readToBuffer ( outData, dataSize );

	assert ( (curPos - startCurPos) == totalAssetSize );

	return true;
}

bool AssetFileReader::endReadAssetFile ()
{
	fclose ( handle );

	if ( curPos != totalBytes )
	{
		printf ( "Warning: asset file premature close. Bytes left %d\n", totalBytes - curPos );
		return false;
	}

	return true;
}
// end Asset File Writer ///////////////////////////////////////////////