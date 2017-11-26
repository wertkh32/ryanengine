#pragma once

#include "universal.h"

#define MIN( x, y )  ( (x) < (y) ? (x) : (y) )
#define MAX( x, y )  ( (x) > (y) ? (x) : (y) )
#define CLAMP( x, a, b ) MAX( a, MIN( b, x ) )
#define TO_RAD( a ) ( ( a ) * ( M_PI / 180. ) )
#define TO_DEG( a ) ( ( a ) * ( 180. / M_PI ) )