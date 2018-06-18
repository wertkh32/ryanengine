// AssetCompiler.cpp : Defines the entry point for the console application.
//
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/postprocess.h>

int main()
{
	Assimp::Importer importer;
	const aiScene* scene;

	scene = importer.ReadFile ( "scenes\\sponza\\sponza.obj",
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded );

    return 0;
}

