#pragma once

#define MIN( x, y )  ( (x) < (y) ? (x) : (y) )
#define MAX( x, y )  ( (x) > (y) ? (x) : (y) )
#define CLAMP( x, a, b ) MAX( a, MIN( b, x ) )