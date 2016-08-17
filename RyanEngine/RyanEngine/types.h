#pragma once
#include "universal.h"

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

typedef float float2[2];
typedef float float3[3];
typedef float float4[4];

#define VERTEXPOS			float3 pos;

#define VERTEXPOSCOL		VERTEXPOS \
							float4 color;

#define VERTEXPOSCOLNORM	VERTEXPOSCOL \
							float3 normal;

struct VertexPos
{
	VERTEXPOS
};

struct VertexPosCol
{
	VERTEXPOSCOL
};

struct VertexPosColNorm
{
	VERTEXPOSCOLNORM
};

#if USE( PLATFORM_WIN32 )

#define _CTX_HANDLE_	HWND
#define _WCHARSTR_		LPWSTR

#endif //#if USE( PLATFORM_WIN32 )