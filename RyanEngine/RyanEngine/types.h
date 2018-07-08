#pragma once
#include <stdint.h>

typedef uint32_t	uint;
typedef uint8_t		uchar;
typedef uint16_t	ushort;
typedef uint32_t	ulong;
typedef uint8_t		byte;

typedef uint32_t	uint32;
typedef uint64_t	uint64;

typedef int32_t		int32;
typedef int64_t		int64;

typedef volatile int32 vint32;
typedef volatile int64 vint64;

typedef float float2[2];
typedef float float3[3];
typedef float float4[4];

typedef struct
{
	byte r, g, b;
} rgb_t;

typedef struct
{
	byte r, g, b, a;
} rgba_t;

typedef struct
{
	byte data[8];
} cblock8_t; // for BC1, BC4


typedef struct
{
	byte data[16];
} cblock16_t; // for BC3, BC5

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