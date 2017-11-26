#pragma once
// place feature specific defines and settings here.

#define IN_USE 7
#define NOT_IN_USE 3
#define USE( x )	( ( IN_USE / ( x ) ) == 1 )

#define PLATFORM_WIN32	IN_USE
#define DX12_API		IN_USE
#define DEBUG_LAYER		IN_USE

