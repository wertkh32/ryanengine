#pragma once
#include "universal.h"

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

#if USE( PLATFORM_WIN32 )

#define _CTX_HANDLE_	HWND
#define _WCHARSTR_		LPWSTR

#endif //#if USE( PLATFORM_WIN32 )