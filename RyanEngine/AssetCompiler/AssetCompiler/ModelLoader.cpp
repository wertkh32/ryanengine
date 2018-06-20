#include "ModelLoader.h"
#include "StackAllocator.h"
#include "Stack.h"
#include "gfx_vertex_streams.h"
#include <assert.h>

#define MAX_NODES 64

ModelLoader::ModelLoader ( const char* fileName )
{
	scene = importer.ReadFile ( fileName,
		aiProcessPreset_TargetRealtime_Fast |
		aiProcess_ConvertToLeftHanded |
		aiProcess_PreTransformVertices );

	assert ( scene );
}


void ModelLoader::processNode ( aiNode * node )
{
	return;
}

void ModelLoader::getModelFromAIScene ( ModelAssetRaw *model, StackAllocator *allocator )
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

			pos_t *xyz = reinterpret_cast<pos_t*>(allocator->allocBytes ( GFX_VERTEX_XYZ_SIZE * surf.vertexCount ));

			for ( uint32_t vindex = 0; vindex < surf.vertexCount; vindex++ )
			{
				xyz[vindex][0] = mesh->mVertices[vindex].x;
				xyz[vindex][1] = mesh->mVertices[vindex].y;
				xyz[vindex][2] = mesh->mVertices[vindex].z;
			}

			surf.vertexBufferDataOffset[GFX_VERTEX_XYZ] = allocator->currentSize - baseOffset;
		}

		
		{ // GFX_VERTEX_UV
			if ( mesh->mTextureCoords[0] != nullptr )
			{
				uv_t *uvs = reinterpret_cast<uv_t*>(allocator->allocBytes ( GFX_VERTEX_UV_SIZE * surf.vertexCount ));

				for ( uint32_t vindex = 0; vindex < surf.vertexCount; vindex++ )
				{
					uvs[vindex][0] = mesh->mTextureCoords[0][vindex].x;
					uvs[vindex][1] = mesh->mTextureCoords[0][vindex].y;
				}

				surf.vertexBufferDataOffset[GFX_VERTEX_UV] = allocator->currentSize - baseOffset;
			}
		}


		{ // GFX_VERTEX_NORMAL
			if ( mesh->mNormals != nullptr )
			{
				axis_t *normals = reinterpret_cast<axis_t*>(allocator->allocBytes ( GFX_VERTEX_NORMAL_SIZE * surf.vertexCount ));

				for ( uint32_t vindex = 0; vindex < surf.vertexCount; vindex++ )
				{
					normals[vindex][0] = mesh->mNormals[vindex].x;
					normals[vindex][1] = mesh->mNormals[vindex].y;
					normals[vindex][2] = mesh->mNormals[vindex].z;
				}

				surf.vertexBufferDataOffset[GFX_VERTEX_NORMAL] = allocator->currentSize - baseOffset;
			}
		}


		{ // GFX_VERTEX_TANGENT
			if ( mesh->mTangents != nullptr )
			{
				axis_t *tangents = reinterpret_cast<axis_t*>( allocator->allocBytes ( GFX_VERTEX_TANGENT_SIZE * surf.vertexCount ) );

				for ( uint32_t vindex = 0; vindex < surf.vertexCount; vindex++ )
				{
					tangents[vindex][0] = mesh->mTangents[vindex].x;
					tangents[vindex][1] = mesh->mTangents[vindex].y;
					tangents[vindex][2] = mesh->mTangents[vindex].z;
				}

				surf.vertexBufferDataOffset[GFX_VERTEX_TANGENT] = allocator->currentSize - baseOffset;
			}
		}


		{ // Indices
			assert ( mesh->HasFaces () );

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

		}
	}
	
}



ModelLoader::~ModelLoader ()
{
	delete scene;
}
