#include "ModelLoader.h"
#include "StackAllocator.h"
#include "Stack.h"
#include "gfx_vertex_streams.h"
#include <assert.h>
#include <string>
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define MAX_NODES 64

ModelLoader::ModelLoader ( const char* fileName )
{
	scene = importer.ReadFile ( fileName,
		aiProcessPreset_TargetRealtime_Fast |
		aiProcess_ConvertToLeftHanded |
		aiProcess_PreTransformVertices );

	strncpy_s ( modelFilePath, fileName, MODEL_PATH_LEN );

	assert ( scene );
}


void ModelLoader::processNode ( aiNode * node )
{
	return;
}


bool ModelLoader::isTextureOnDisk ( const char* filePath )
{
	return ( strrchr( filePath, '.' ) != NULL );
}


void ModelLoader::getModelFromAIScene ( StackAllocator *allocator )
{
	ModelAssetRaw *modelAsset;
	ModelLODAssetRaw *modelLODAsset;
	SurfaceAssetRaw *surfaceAssets;
	
	uint32_t baseOffset;

	baseOffset = allocator->currentSize;

	modelAsset = reinterpret_cast< ModelAssetRaw* >( allocator->allocBytes ( sizeof ( ModelAssetRaw ) ) );
	modelAsset->numLods = 1; 	//Only 1 lod for now
	modelAsset->modelLODsDataOffset = allocator->currentSize - baseOffset;
	
	modelLODAsset = reinterpret_cast< ModelLODAssetRaw* >( allocator->allocBytes ( sizeof ( ModelLODAssetRaw ) ) );
	modelLODAsset->numSurfaces = scene->mNumMeshes;
	modelLODAsset->surfacesDataOffset = allocator->currentSize - baseOffset;

	surfaceAssets = reinterpret_cast< SurfaceAssetRaw* >( allocator->allocBytes ( sizeof ( SurfaceAssetRaw ) * modelLODAsset->numSurfaces ) );

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

			surf.vertexBufferDataOffset[GFX_VERTEX_XYZ] = allocator->currentSize - baseOffset;

			pos_t *xyz = reinterpret_cast<pos_t*>(allocator->allocBytes ( GFX_VERTEX_XYZ_SIZE * surf.vertexCount ));

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
				surf.vertexBufferDataOffset[GFX_VERTEX_UV] = allocator->currentSize - baseOffset;

				uv_t *uvs = reinterpret_cast<uv_t*>(allocator->allocBytes ( GFX_VERTEX_UV_SIZE * surf.vertexCount ));

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
				surf.vertexBufferDataOffset[GFX_VERTEX_NORMAL] = allocator->currentSize - baseOffset;

				axis_t *normals = reinterpret_cast<axis_t*>(allocator->allocBytes ( GFX_VERTEX_NORMAL_SIZE * surf.vertexCount ));

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
				surf.vertexBufferDataOffset[GFX_VERTEX_TANGENT] = allocator->currentSize - baseOffset;

				axis_t *tangents = reinterpret_cast<axis_t*>( allocator->allocBytes ( GFX_VERTEX_TANGENT_SIZE * surf.vertexCount ) );

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

			surf.indexBufferDataOffset = allocator->currentSize - baseOffset;

			index_t *indices = reinterpret_cast<index_t*>( allocator->allocBytes ( sizeof( index_t ) * surf.indexCount ) );

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
	modelAsset->materialsDataOffset = allocator->currentSize - baseOffset;

	MaterialAssetRaw *materialsRaw = reinterpret_cast< MaterialAssetRaw* >(allocator->allocBytes ( sizeof ( MaterialAssetRaw ) * modelAsset->numMaterials ));

	for ( uint i = 0; i < scene->mNumMaterials; i++ )
	{
		aiMaterial* material = scene->mMaterials[i];

		materialsRaw[i].numShaders = 0; // for now
		materialsRaw[i].numTextures = 0; // init
		materialsRaw[i].shadersDataOffset = INVALID_DATA_OFFSET; // for now
		materialsRaw[i].texturesDataOffset = allocator->currentSize - baseOffset;

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

				rgb_t *textureData = reinterpret_cast< rgb_t* >( stbi_load ( correctedPath, &width, &height, &components, 3 ) );

				if ( !textureData )
				{
					printf ( "Loading %s failed\n", correctedPath );
					goto invalid_texture;
				}

				const char *name = strrchr ( cpath, '\\' );

				if ( name )
					name++;
				else
					name = cpath;

				uint nameLength = strnlen ( name, ASSET_NAME_LENGTH );
				
				assert ( nameLength != ASSET_NAME_LENGTH );
				
				printf ( "Texture name is %s\n", name );

				TextureAssetRaw *textureAsset = reinterpret_cast<TextureAssetRaw*>(allocator->allocBytes ( sizeof ( TextureAssetRaw ) ));
				strncpy_s ( textureAsset->name, name, ASSET_NAME_LENGTH );
				
				textureAsset->width = static_cast<uint32_t>(width);
				textureAsset->height = static_cast<uint32_t>(height);
				textureAsset->pixelFormat = 0; // populate later. now everything is just rgb8
				textureAsset->pixelSize = sizeof ( rgb_t );
				textureAsset->textureDataOffset = allocator->currentSize - baseOffset;
				
				uint textureSizeInBytes = sizeof ( rgb_t ) * width * height;
				
				rgb_t *fileTextureData = reinterpret_cast<rgb_t*>(allocator->allocBytes ( textureSizeInBytes ));
				
				if ( !fileTextureData )
				{
					printf ( "Failed allocating texture data for %s on file buffer", name );
					goto invalid_texture;
				}

				memcpy ( fileTextureData, textureData, textureSizeInBytes );
				
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
