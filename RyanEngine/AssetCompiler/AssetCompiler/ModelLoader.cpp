#include "ModelLoader.h"
#include "StackAllocator.h"
#include "Stack.h"
#include "gfx_vertex_streams.h"
#include <assert.h>
#include <string>
#include <cstring>
#include "string_utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_DXT_IMPLEMENTATION
#include "stb_dxt.h"

#define MAX_NODES 64
#define BLOCK_COUNT( x, n ) ( ( (x) + ( n - 1 ) ) / ( n ) )


ModelLoader::ModelLoader ( const char* fileName, StackAllocator *alloc )
{
	scene = importer.ReadFile ( fileName,
		aiProcessPreset_TargetRealtime_Fast |
		aiProcess_ConvertToLeftHanded |
		aiProcess_PreTransformVertices );

	strncpy_s ( modelFilePath, fileName, MODEL_PATH_LEN );

	allocator = alloc;
	baseOffset = alloc->currentSize;

	assert ( scene );
}


uint32_t ModelLoader::getCurrentOffset ()
{
	return allocator->currentSize - baseOffset;
}


bool ModelLoader::isTextureOnDisk ( const char* filePath )
{
	return ( strrchr( filePath, '.' ) != NULL );
}

bool ModelLoader::processTextureAsBC1 ( rgba_t * inputTexture, uint32_t width, uint32_t height )
{
	uint widthBlocks = BLOCK_COUNT ( width, 4 );
	uint heightBlocks = BLOCK_COUNT ( height, 4 );

	uint numBlocks = widthBlocks * heightBlocks;
	
	cblock8_t *fileTextureData = allocateFileMem< cblock8_t > ( numBlocks );
	
	printf ( "Compressing texture to BC1\n" );

	if ( !fileTextureData )
	{
		printf ( "Failed allocating texture data on file buffer" );
		return false;
	}

	for ( uint y = 0; y < heightBlocks; y++ )
	{
		for ( uint x = 0; x < widthBlocks; x++ )
		{
			rgba_t inputBlock[4][4];
			cblock8_t compressedBlock;

			for ( uint by = 0; by < 4; by++ )
			{
				for ( uint bx = 0; bx < 4; bx++ )
				{
					uint pix_x = x * 4 + bx;
					uint pix_y = y * 4 + by;

					// if width/height not a multiple of 4, just clamp it.
					if ( pix_x >= width && pix_y >= height )
						inputBlock[by][bx] = inputTexture[ ( height - 1 ) * width + ( width - 1 ) ];
					else if ( pix_x >= width )
						inputBlock[by][bx] = inputTexture[ pix_y * width + (width - 1)];
					else if( pix_y >= height )
						inputBlock[by][bx] = inputTexture[ ( height - 1 )* width + ( pix_x ) ];
					else
						inputBlock[by][bx] = inputTexture[ ( pix_y )* width + ( pix_x ) ];
				}
			}

			stb_compress_dxt_block ( reinterpret_cast< unsigned char* >( &compressedBlock ), 
									 reinterpret_cast< unsigned char* >( inputBlock ),
									 0, 0 );

			fileTextureData[ y * widthBlocks + x ] = compressedBlock;
		}
	}

	printf ( "Compressed Texture to BC1, original size: %d, compressed size %d\n", width * height * sizeof ( rgba_t ), numBlocks * sizeof ( cblock8_t ) );

	return true;
}


void ModelLoader::getModelFromAIScene ()
{
	ModelAssetRaw *modelAsset;
	ModelLODAssetRaw *modelLODAsset;
	SurfaceAssetRaw *surfaceAssets;
	
	assert( baseOffset == allocator->currentSize );

	modelAsset = allocateFileMem< ModelAssetRaw >( 1 );
	modelAsset->numLods = 1; 	//Only 1 lod for now
	modelAsset->modelLODsDataOffset = getCurrentOffset ();
	
	modelLODAsset = allocateFileMem< ModelLODAssetRaw > ( 1 );
	modelLODAsset->numSurfaces = scene->mNumMeshes;
	modelLODAsset->surfacesDataOffset = getCurrentOffset ();

	surfaceAssets = allocateFileMem< SurfaceAssetRaw > ( modelLODAsset->numSurfaces );

	for ( uint32_t i = 0; i < scene->mNumMeshes; i++ )
	{
		aiMesh* mesh = scene->mMeshes[i];
		SurfaceAssetRaw &surf = surfaceAssets[i];

		assert ( mesh->mNumVertices <= MESH_MAX_VERTICES );

		surf.vertexCount = mesh->mNumVertices;
		surf.indexCount = mesh->mNumFaces * 3; // face must be triangles

		for ( uint32_t vtype = 0; vtype < GFX_VERTEX_COUNT; vtype++ )
			surf.vertexBufferDataOffset[vtype] = INVALID_DATA_OFFSET;

		{ // GFX_VERTEX_XYZ
			assert ( mesh->HasPositions() );

			surf.vertexBufferDataOffset[GFX_VERTEX_XYZ] = getCurrentOffset ();

			pos_t *xyz = allocateFileMem< pos_t >( surf.vertexCount );

			for ( uint32_t vindex = 0; vindex < surf.vertexCount; vindex++ )
			{
				xyz[vindex][0] = mesh->mVertices[vindex].x;
				xyz[vindex][1] = mesh->mVertices[vindex].y;
				xyz[vindex][2] = mesh->mVertices[vindex].z;
			}
		}

		
		{ // GFX_VERTEX_UV
			if ( mesh->mTextureCoords[0] != nullptr )
			{
				surf.vertexBufferDataOffset[GFX_VERTEX_UV] = getCurrentOffset ();

				uv_t *uvs = allocateFileMem< uv_t > ( surf.vertexCount );

				for ( uint32_t vindex = 0; vindex < surf.vertexCount; vindex++ )
				{
					uvs[vindex][0] = mesh->mTextureCoords[0][vindex].x;
					uvs[vindex][1] = mesh->mTextureCoords[0][vindex].y;
				}
			}
		}


		{ // GFX_VERTEX_NORMAL
			if ( mesh->mNormals != nullptr )
			{
				surf.vertexBufferDataOffset[GFX_VERTEX_NORMAL] = getCurrentOffset ();

				axis_t *normals = allocateFileMem< axis_t > ( surf.vertexCount );

				for ( uint32_t vindex = 0; vindex < surf.vertexCount; vindex++ )
				{
					normals[vindex][0] = mesh->mNormals[vindex].x;
					normals[vindex][1] = mesh->mNormals[vindex].y;
					normals[vindex][2] = mesh->mNormals[vindex].z;
				}
			}
		}


		{ // GFX_VERTEX_TANGENT
			if ( mesh->mTangents != nullptr )
			{
				surf.vertexBufferDataOffset[GFX_VERTEX_TANGENT] = getCurrentOffset ();

				axis_t *tangents = allocateFileMem< axis_t > ( surf.vertexCount );

				for ( uint32_t vindex = 0; vindex < surf.vertexCount; vindex++ )
				{
					tangents[vindex][0] = mesh->mTangents[vindex].x;
					tangents[vindex][1] = mesh->mTangents[vindex].y;
					tangents[vindex][2] = mesh->mTangents[vindex].z;
				}
			}
		}


		{ // Indices
			assert ( mesh->HasFaces () );

			surf.indexBufferDataOffset = getCurrentOffset ();

			index_t *indices = allocateFileMem< index_t > ( surf.indexCount );

			for ( uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++ )
			{
				assert ( mesh->mFaces[faceIndex].mNumIndices == 3 ); // must be triangles.
			
				uint32_t index0 = mesh->mFaces[faceIndex].mIndices[0];
				uint32_t index1 = mesh->mFaces[faceIndex].mIndices[1];
				uint32_t index2 = mesh->mFaces[faceIndex].mIndices[2];

				assert ( index0 < MESH_MAX_VERTICES );
				assert ( index1 < MESH_MAX_VERTICES );
				assert ( index2 < MESH_MAX_VERTICES );

				indices[faceIndex * 3 + 0] = static_cast<index_t>( index0 );
				indices[faceIndex * 3 + 1] = static_cast<index_t>( index1 );
				indices[faceIndex * 3 + 2] = static_cast<index_t>( index2 );
			}
		}

		
		{ // load material
			assert ( mesh->mMaterialIndex < scene->mNumMaterials );

			surf.materialIndex = mesh->mMaterialIndex;
		}
	}


	modelAsset->numMaterials = scene->mNumMaterials;
	modelAsset->materialsDataOffset = getCurrentOffset ();

	MaterialAssetRaw *materialsRaw = allocateFileMem< MaterialAssetRaw > ( modelAsset->numMaterials );

	for ( uint i = 0; i < scene->mNumMaterials; i++ )
	{
		aiMaterial* material = scene->mMaterials[i];

		materialsRaw[i].numShaders = 0; // for now
		materialsRaw[i].numTextures = 0; // init
		materialsRaw[i].shadersDataOffset = INVALID_DATA_OFFSET; // for now
		materialsRaw[i].texturesDataOffset = getCurrentOffset ();

		printf ( "Loading Material %d\n", i );

		{
			aiTextureType type = aiTextureType_DIFFUSE;
			uint textureCount = material->GetTextureCount ( type );

			printf ( "Material %d has %d textures\n", i, textureCount );

			if( textureCount ) // only load 1 texture for now
			{
				//layered materials (multiple textures of same type) not supported yet
				char correctedPath[MODEL_PATH_LEN];
				aiString path;
				material->GetTexture ( type, 0, &path );

				const char* cpath = path.C_Str ();
				
				printf ( "Found texture at %s\n", cpath );

				assert ( isTextureOnDisk ( cpath ) );

				materialsRaw[i].numTextures++;

				// allocate texture asset struct
				// allocate image data

				int width;
				int height;
				int components;

				{
					const char *currentRelDir = strrchr ( modelFilePath, '\\' );
					assert ( currentRelDir );
					int relDirLen = currentRelDir - modelFilePath + 1;
					assert ( relDirLen >= 0 );
					strncpy ( correctedPath, modelFilePath, relDirLen );
					correctedPath[relDirLen] = '\0';
					strncat ( correctedPath + relDirLen, cpath, MODEL_PATH_LEN );
				}

				printf ( "Loading texture at path %s\n", correctedPath );

				rgba_t *textureData = reinterpret_cast< rgba_t* >( stbi_load ( correctedPath, &width, &height, &components, 4 ) );

				if ( !textureData )
				{
					printf ( "Loading %s failed\n", correctedPath );
					goto invalid_texture;
				}

				const char *name = GetFileNameFromPath ( cpath );

				uint nameLength = strnlen ( name, ASSET_NAME_LENGTH );
				
				assert ( nameLength != ASSET_NAME_LENGTH );
				
				printf ( "Texture name is %s\n", name );

				TextureAssetRaw *textureAsset = reinterpret_cast<TextureAssetRaw*>(allocator->allocBytes ( sizeof ( TextureAssetRaw ) ));
				strncpy_s ( textureAsset->name, name, ASSET_NAME_LENGTH );
				
				textureAsset->width = static_cast<uint32_t>(width);
				textureAsset->height = static_cast<uint32_t>(height);
				textureAsset->pixelFormat = GFX_TEX_PIXEL_SIZE_BC1; // for now since we are only touching diffuse textures
				textureAsset->pixelSize = GFX_TEX_FORMAT_BC1; // for now since we are only touchying diffuse textures
				textureAsset->textureDataOffset = getCurrentOffset ();
				
				if ( !processTextureAsBC1 ( textureData, width, height ) )
					goto invalid_texture;

				stbi_image_free ( textureData );			
			}
			else

invalid_texture:
				materialsRaw[i].texturesDataOffset = INVALID_DATA_OFFSET;
		}
	}
	
}



ModelLoader::~ModelLoader ()
{
	delete scene;
}
