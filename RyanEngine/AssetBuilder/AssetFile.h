#pragma once
#include "asset_structs.h"
#include <stdio.h>
#include <stdint.h>

#define ASSET_FILE_MAGIC 0xBABECAFE
#define ASSET_FILE_NAME_SIZE 64

struct AssetFileHeader
{
	uint32_t magic;
	uint32_t checksum; // need 64 bit or this shit gonna hash collide
	uint32_t assetFileSize;
	uint32_t assetCount;
};
sassert( sizeof ( AssetFileHeader ) );

struct AssetHeader
{
	uint32_t size; // size = AssetHeader + dataSize
	uint32_t assetType;
	uint32_t dataSize;
};


class AssetFileWriter
{
	const char* filePath;
	AssetFileHeader header;
	FILE *handle;
	uint32_t curPos;

	bool beginWrite;
	void updateCurPos ( uint32_t inDataSize );
	void writeToCurrentFile ( const void * data, uint32_t inDataSize );
public:
	AssetFileWriter ();
	~AssetFileWriter ();

	bool beginWriteAssetFile ( const char* filePath );
	bool addAsset ( ASSET_TYPE type, const void *inData, uint32_t inDataSize );
	bool endWriteAssetFile ();
};


class AssetFileReader
{
	const char* filePath;
	AssetFileHeader header;
	FILE *handle;
	uint32_t totalBytes;
	uint32_t curPos;

	void updateCurPos ( uint32_t inDataSize );
	void readToBuffer ( void * data, uint32_t inDataSize );
public:
	AssetFileReader ();
	~AssetFileReader ();

	bool beginReadAssetFile ( const char* filePath );
	bool getNextAsset ( ASSET_TYPE *outType, void *outData, uint32_t outDataBufferSize, uint32_t *outDataSize );
	bool endReadAssetFile ();
};

