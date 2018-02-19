#pragma once
#include "universal.h"

typedef unsigned int	uint;
typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef unsigned long	ulong;
typedef uchar			byte;

typedef unsigned long		uint32;
typedef unsigned long long	uint64;

typedef long		int32;
typedef long long	int64;

typedef volatile int32 v_int32;
typedef volatile int64 v_int64;

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

typedef HWND ctx_handle_t;
typedef LPWSTR _WCHARSTR_;

typedef float	float_t;

#endif //#if USE( PLATFORM_WIN32 )