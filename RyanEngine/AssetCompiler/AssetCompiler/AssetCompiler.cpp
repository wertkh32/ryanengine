// AssetCompiler.cpp : Defines the entry point for the console application.
//
#include <stdlib.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include "SceneCompiler.h"
#include "SceneLoader.h"
#include "StackAllocator.h"

#define BUF_SIZE ( 128 * 1024 * 1024 )
byte fileBuffer[BUF_SIZE];

int main()
{
	//Assimp::Importer importer;
	//const aiScene* scene;
	//
	//scene = importer.ReadFile ( "scenes\\sponza\\sponza.obj",
	//	aiProcess_Triangulate |
	//	aiProcess_ConvertToLeftHanded );

	StackAllocator alloc ( fileBuffer, BUF_SIZE );
	SceneCompiler loader( "scenes\\rock\\Rock1.obj",&alloc );
	loader.getModelFromAIScene ();

	printf ( "File buffer loaded, bytes used: %u", alloc.currentSize );

	SceneLoader reader ( alloc.array, alloc.currentSize );
	reader.FixupSceneFromSceneRaw ();

	system ( "pause" );

    return 0;
}

