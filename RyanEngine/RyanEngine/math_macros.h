#pragma once

#include "universal.h"

#define MIN( x, y )  ( (x) < (y) ? (x) : (y) )
#define MAX( x, y )  ( (x) > (y) ? (x) : (y) )
#define CLAMP( x, a, b ) MAX( a, MIN( b, x ) )
#define TO_RAD( a ) ( ( a ) * ( static_cast< float >( M_PI ) / 180.0f ) )
#define TO_DEG( a ) ( ( a ) * ( 180.0f / static_cast< float >( M_PI ) ) )


constexpr uint COUNT_LEADING_ZEROS_CONST ( uint n )
{
	return ((n & 0x80000000) == 0x80000000) ? 0 : COUNT_LEADING_ZEROS_CONST ( n << 1 ) + 1;
}


constexpr uint COUNT_TRAILING_ZEROS_CONST ( uint n )
{
	return ( ( n & 1 ) == 1 ) ? 0 : COUNT_TRAILING_ZEROS_CONST( n >> 1 ) + 1;
}