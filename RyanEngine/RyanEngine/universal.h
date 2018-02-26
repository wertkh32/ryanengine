#pragma once
// place universal defines and common api includes here

#include "feature.h"
#include "platform.h"
#include "types.h"

#define _USE_MATH_DEFINES

#include <math.h>
#include <string>
#include <assert.h>
#include <intrin.h>
#include <xmmintrin.h>
#include <smmintrin.h>
#include "math_macros.h"
#include "intrinsics.h"

enum DISPLAY_RESOLUTIONS
{
	DRES_720p,
	DRES_900p,
	DRES_1080p
};


#define WINDOWED_WIDTH 1280
#define WINDOWED_HEIGHT 720
#define GAME_TITLE	"RYAN ENGINE"
#define NUM_FRAMEBUFFERS 2


#define ALIGN( x ) __declspec( align( x ) )
#define ALIGN_UP( x, n ) ( ( (x) + ( (n) - 1 ) ) & ~( (n) - 1 ) )
#define IS_ALIGNED( x, n ) ( ( (x) & ( (n) - 1) ) == 0 )
#define IS_ALIGNED_PTR( x, n ) ( ( reinterpret_cast<uintptr_t>(x) & ( (n) - 1) ) == 0 )
#define NUM_BLOCKS( x, n ) ( ( (x) + ( (n) - 1 ) ) / (n) )
#define IS_POWER_2( x ) ( ( (x) & ( (x) - 1 ) ) == 0 )
#define sassert( x ) static_assert( ( x ), #x )

#define KB ( 1024U )
#define MB ( KB * KB )

// CPU side matrices are row major. GPU side matrices are column major

// add CRIP classes here
template<class T>
class Singleton
{
public:
	static T* Instance ()
	{
		static T* instance = 0;
		// instantiate only once... no more
		if ( !instance )
			instance = new T ();
		return instance;
	}
};