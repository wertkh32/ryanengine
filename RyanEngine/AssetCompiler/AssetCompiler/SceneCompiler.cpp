#include "SceneCompiler.h"
#include "StackAllocator.h"
#include "gfx_vertex_streams.h"
#include <assert.h>
#include <string>
#include <cstring>
#include <map>
#include "string_utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_DXT_IMPLEMENTATION
#include "stb_dxt.h"

#define MAX_NODES 64
#define BLOCK_COUNT( x, n ) ( ( (x) + ( n - 1 ) ) / ( n ) )


SceneCompiler::SceneCompiler ( const char* fileName, StackAllocator *alloc )
{
	scene = importer.ReadFile ( fileName,
		aiProcessPreset_TargetRealtime_Fast |
		aiProcess_ConvertToLeftHanded |
		aiProcess_PreTransformVertices );

	strncpy_s ( sceneFilePath, fileName, MODEL_PATH_LEN );

	allocator = alloc;
	baseOffset = alloc->currentSize;

	assert ( scene );
}


uint32_t SceneCompiler::getCurrentOffset ()
{
	return allocator->currentSize - baseOffset;
}


bool SceneCompiler::isTextureOnDisk ( const char* filePath )
{
	return ( strrchr( filePath, '.' ) != NULL );
}

bool SceneCompiler::processTextureAsBC1 ( rgba_t * inputTexture, uint32_t width, uint32_t height )
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

	printf ( "Compressed Texture to BC1, original size: %zu, compressed size %zu\n", width * height * sizeof ( rgba_t ), numBlocks * sizeof ( cblock8_t ) );

	return true;
}


void SceneCompiler::ExtractModelsFromScene()
{
	assert( modelStack.currentSize==0 );
	{
		StackArray<const aiNode*, MAX_MODELS> nodeStack;

		const aiNode *root = scene->mRootNode;
		nodeStack.push( root );

		assert( nodeStack.currentSize==1 );
		while( nodeStack.currentSize )
		{
			const aiNode *node = nodeStack.pop();

			if( node->mNumChildren )
			{
				// parent node
				for( uint childIndex = 0; childIndex<node->mNumChildren; childIndex++ )
				{
					nodeStack.push( node->mChildren[childIndex] );
				}
			}
			else
			{
				//leaf node
				modelStack.push( node );
			}

		}
	}

	assert( modelStack.currentSize > 0 );

	sassert( sizeof( aiMatrix4x4 )==64 );
	sassert( sizeof( mat4x4 )==64 );

	assert( transformStack.currentSize == 0 );
	{
		for( uint modelIndex = 0; modelIndex < modelStack.currentSize; modelIndex++ )
		{
			const aiNode *model = modelStack.array[modelIndex];
			const aiNode *nextModel = model;

			aiMatrix4x4 transform;

			while( nextModel->mParent )
			{
				transform = nextModel->mTransformation * transform;
				nextModel = nextModel->mParent;
			}

			assert( nextModel==scene->mRootNode );
			assert( transform.Determinant() > 0 );
			transformStack.push( reinterpret_cast< const mat4x4& >( transform ) );
		}
	}

	assert( modelStack.currentSize==transformStack.currentSize );
}


uint SceneCompiler::GetSceneTextureCount()
{
	uint sceneTextureCount = 0;

	for( uint i = 0; i < scene->mNumMaterials; i++ )
	{
		aiMaterial* srcMaterial = scene->mMaterials[i];

		{
			aiTextureType type = aiTextureType_DIFFUSE;
			uint textureCount = srcMaterial->GetTextureCount( type );

			printf( "Texture Counter: Material %d has %d diffuse textures\n", i, textureCount );

			if( textureCount )
			{
				sceneTextureCount++;
			}
		}
	}

	return sceneTextureCount;
}


void SceneCompiler::getModelFromAIScene ()
{
	assert( baseOffset == allocator->currentSize );

	SceneRaw *sceneAsset;

	sceneAsset = allocateFileMem< SceneRaw >( 1 );
	
	ExtractModelsFromScene();

	assert( modelStack.currentSize );

	uint sceneTextureCount = GetSceneTextureCount();

	assert( sceneTextureCount );

	sceneAsset->numModels = modelStack.currentSize;
	sceneAsset->numSurfaces = scene->mNumMeshes;
	sceneAsset->numMaterials = scene->mNumMaterials;
	sceneAsset->numTextures = sceneTextureCount;
	sceneAsset->numShaders = 0; // for now

	assert( sceneAsset->numModels < MAX_MODELS );
	assert( sceneAsset->numSurfaces < MAX_SURFACES );
	assert( sceneAsset->numMaterials < MAX_MATERIALS );
	assert( sceneAsset->numTextures <  MAX_TEXTURES );
	assert( sceneAsset->numShaders <  MAX_TEXTURES );

	// follow struct order
	sceneAsset->modelsOffset = getCurrentOffset();
	ModelAssetRaw *modelAssets = allocateFileMem< ModelAssetRaw >( sceneAsset->numModels );

	sceneAsset->surfacesOffset = getCurrentOffset();
	SurfaceAssetRaw *surfaceAssets = allocateFileMem< SurfaceAssetRaw >( sceneAsset->numSurfaces );

	sceneAsset->materialsOffset = getCurrentOffset();
	MaterialAssetRaw *materialAssets = allocateFileMem< MaterialAssetRaw >( sceneAsset->numMaterials );

	sceneAsset->texturesOffset = getCurrentOffset();
	TextureAssetRaw *textureAssets = allocateFileMem< TextureAssetRaw >( sceneAsset->numTextures );

	sceneAsset->modelTransformsOffset = getCurrentOffset();
	mat4x4 *modelTransforms = allocateFileMem< mat4x4 >( sceneAsset->numModels );

	// todo shader assets

	{ // models
	  // copy transforms
		memcpy( modelTransforms, transformStack.array, sizeof( mat4x4 ) * sceneAsset->numModels );

		for( uint32_t modelIndex = 0; modelIndex < sceneAsset->numModels; modelIndex++ )
		{
			ModelAssetRaw& model = modelAssets[modelIndex];
			const aiNode *aiModel = modelStack.array[modelIndex];

			assert( aiModel->mNumChildren == 0 );

			model.numSurfaces = aiModel->mNumMeshes; // for now since there is only 1 model

			model.surfaceIndicesOffset = getCurrentOffset();
			SurfaceID *surfaceIndices = allocateFileMem< SurfaceID >( model.numSurfaces );

			model.surfaceToMaterialMapOffset = getCurrentOffset();
			MaterialID *surfaceToMaterialMap = allocateFileMem< MaterialID >( model.numSurfaces );

			for( uint32_t surfaceIndex = 0; surfaceIndex<scene->mNumMeshes; surfaceIndex++ )
			{
				aiMesh* mesh = scene->mMeshes[surfaceIndex];

				surfaceIndices[surfaceIndex] = static_cast< SurfaceID >( surfaceIndex );
				surfaceToMaterialMap[surfaceIndex] = static_cast< MaterialID >( mesh->mMaterialIndex );
			}
		}
	}


	{ // surfaces
		for( uint32_t i = 0; i<scene->mNumMeshes; i++ )
		{
			aiMesh* mesh = scene->mMeshes[i];
			SurfaceAssetRaw &surf = surfaceAssets[i];

			assert( mesh->mNumVertices<=MESH_MAX_VERTICES );

			surf.vertexCount = mesh->mNumVertices;
			surf.indexCount = mesh->mNumFaces*3; // face must be triangles

			for( uint32_t vtype = 0; vtype<GFX_VERTEX_COUNT; vtype++ )
				surf.vertexBufferOffset[vtype] = INVALID_DATA_OFFSET;

			{ // GFX_VERTEX_XYZ
				assert( mesh->HasPositions() );

				surf.vertexBufferOffset[GFX_VERTEX_XYZ] = getCurrentOffset();

				pos_t *xyz = allocateFileMem< pos_t >( surf.vertexCount );

				for( uint32_t vindex = 0; vindex<surf.vertexCount; vindex++ )
				{
					xyz[vindex][0] = mesh->mVertices[vindex].x;
					xyz[vindex][1] = mesh->mVertices[vindex].y;
					xyz[vindex][2] = mesh->mVertices[vindex].z;
				}
			}


			{ // GFX_VERTEX_UV
				if( mesh->mTextureCoords[0]!=nullptr )
				{
					surf.vertexBufferOffset[GFX_VERTEX_UV] = getCurrentOffset();

					uv_t *uvs = allocateFileMem< uv_t >( surf.vertexCount );

					for( uint32_t vindex = 0; vindex<surf.vertexCount; vindex++ )
					{
						uvs[vindex][0] = mesh->mTextureCoords[0][vindex].x;
						uvs[vindex][1] = mesh->mTextureCoords[0][vindex].y;
					}
				}
			}


			{ // GFX_VERTEX_NORMAL // todo merge with tangent into quaternions later
				if( mesh->mNormals!=nullptr )
				{
					surf.vertexBufferOffset[GFX_VERTEX_NORMAL] = getCurrentOffset();

					axis_t *normals = allocateFileMem< axis_t >( surf.vertexCount );

					for( uint32_t vindex = 0; vindex<surf.vertexCount; vindex++ )
					{
						normals[vindex][0] = mesh->mNormals[vindex].x;
						normals[vindex][1] = mesh->mNormals[vindex].y;
						normals[vindex][2] = mesh->mNormals[vindex].z;
					}
				}
			}


			{ // GFX_VERTEX_TANGENT
				if( mesh->mTangents!=nullptr )
				{
					surf.vertexBufferOffset[GFX_VERTEX_TANGENT] = getCurrentOffset();

					axis_t *tangents = allocateFileMem< axis_t >( surf.vertexCount );

					for( uint32_t vindex = 0; vindex<surf.vertexCount; vindex++ )
					{
						tangents[vindex][0] = mesh->mTangents[vindex].x;
						tangents[vindex][1] = mesh->mTangents[vindex].y;
						tangents[vindex][2] = mesh->mTangents[vindex].z;
					}
				}
			}


			{ // Indices
				assert( mesh->HasFaces() );

				surf.indexBufferOffset = getCurrentOffset();

				index_t *indices = allocateFileMem< index_t >( surf.indexCount );

				for( uint32_t faceIndex = 0; faceIndex<mesh->mNumFaces; faceIndex++ )
				{
					assert( mesh->mFaces[faceIndex].mNumIndices==3 ); // must be triangles.

					uint32_t index0 = mesh->mFaces[faceIndex].mIndices[0];
					uint32_t index1 = mesh->mFaces[faceIndex].mIndices[1];
					uint32_t index2 = mesh->mFaces[faceIndex].mIndices[2];

					assert( index0<MESH_MAX_VERTICES );
					assert( index1<MESH_MAX_VERTICES );
					assert( index2<MESH_MAX_VERTICES );

					indices[faceIndex*3+0] = static_cast< index_t >( index0 );
					indices[faceIndex*3+1] = static_cast< index_t >( index1 );
					indices[faceIndex*3+2] = static_cast< index_t >( index2 );
				}
			}
		}
	}


	// todo: materials -> take out texture processing from material processing
	// todo: textures
	std::map<aiString, TextureAssetRaw> textureTable;

	uint loadedTextureCount = 0;
	for ( uint i = 0; i < scene->mNumMaterials; i++ )
	{
		aiMaterial* srcMaterial = scene->mMaterials[i];

		materialAssets[i].numShaders = 0; // for now
		materialAssets[i].numTextures = 0; // init
		materialAssets[i].shaderIndicesOffset = INVALID_DATA_OFFSET; // for now
		materialAssets[i].textureIndicesOffset = INVALID_DATA_OFFSET;

		uint baseTextureIndex = loadedTextureCount;

		printf ( "Loading Material %d\n", i );
		
		{ // load textures for material
			aiTextureType type = aiTextureType_DIFFUSE;
			uint textureCount = srcMaterial->GetTextureCount( type );

			printf( "Material %d has %d textures\n", i, textureCount );

			if( textureCount ) // only load 1 texture for now
			{
				//layered materials (multiple textures of same type) not supported yet
				char correctedPath[MODEL_PATH_LEN];
				aiString path;
				srcMaterial->GetTexture( type, 0, &path ); // load first diffuse texture

				const char* cpath = path.C_Str();

				printf( "Found texture at %s\n", cpath );

				assert( isTextureOnDisk( cpath ) );

				// allocate texture asset struct
				// allocate image data

				int width;
				int height;
				int components;

				{	// get real path
					const char *currentRelDir = strrchr( sceneFilePath, '\\' );
					assert( currentRelDir );
					int relDirLen = static_cast< int >( currentRelDir-sceneFilePath+1 );
					assert( relDirLen>=0 );
					strncpy( correctedPath, sceneFilePath, relDirLen );
					correctedPath[relDirLen] = '\0';
					strncat( correctedPath+relDirLen, cpath, MODEL_PATH_LEN );
				}

				printf( "Loading texture at path %s\n", correctedPath );

				rgba_t *textureData = reinterpret_cast< rgba_t* >( stbi_load( correctedPath, &width, &height, &components, 4 ) );

				assert( textureData );

				{	// load texture from file
					const char *name = GetFileNameFromPath( cpath );

					uint nameLength = static_cast< uint >( strnlen( name, ASSET_NAME_LENGTH ) );

					assert( nameLength!=ASSET_NAME_LENGTH );

					printf( "Texture name is %s\n", name );

					TextureAssetRaw &textureAsset = textureAssets[loadedTextureCount];
					strncpy_s( textureAsset.name, name, ASSET_NAME_LENGTH );

					textureAsset.width = static_cast< uint32_t >( width );
					textureAsset.height = static_cast< uint32_t >( height );
					textureAsset.pixelFormat = GFX_TEX_PIXEL_SIZE_BC1; // for now since we are only touching diffuse textures
					textureAsset.pixelSize = GFX_TEX_FORMAT_BC1; // for now since we are only touchying diffuse textures
					textureAsset.textureDataOffset = getCurrentOffset();

					bool processed = processTextureAsBC1( textureData, width, height );

					assert( processed );

					stbi_image_free( textureData );
				}

				materialAssets[i].numTextures++;
				loadedTextureCount++;
			}
		}

		if( materialAssets[i].numTextures )
		{
			materialAssets[i].textureIndicesOffset = getCurrentOffset();
			TextureID *textureIndices = allocateFileMem< TextureID >( materialAssets[i].numTextures );

			for( uint textureIter = 0; textureIter<materialAssets[i].numTextures; textureIter++ )
			{
				textureIndices[textureIter] = baseTextureIndex+textureIter;
			}
		}
	}
	
	assert( loadedTextureCount == sceneTextureCount );

	printf ( "\nScene Compiled! Scene has %d models\n", sceneAsset->numModels );
	printf ( "%d surfaces\n", sceneAsset->numSurfaces );
	printf ( "%d materials\n", sceneAsset->numMaterials );
	printf ( "%d textures\n", sceneAsset->numTextures );
}



SceneCompiler::~SceneCompiler ()
{
	//delete scene;
}
