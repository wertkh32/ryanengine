#pragma once
// place universal defines and common api includes here

#include "feature.h"
#include "platform.h"
#include "types.h"

#define _USE_MATH_DEFINES

#include <math.h>
#include <string>
#include <assert.h>
#include <xmmintrin.h>
#include <smmintrin.h>
#include "math_macros.h"


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
#define NEAREST_MULTIPLE( x, n ) ( ( (x) + ( (n) - 1 ) ) & ~( (n) - 1 ) )

// CPU side matrices are row major. GPU side matrices are column major