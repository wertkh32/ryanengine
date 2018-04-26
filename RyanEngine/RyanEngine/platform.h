#pragma once
// put platform defines and settings ( platform is defined in feature.h ) here.

#include "feature.h"

#if USE( PLATFORM_WIN32 )

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

#include <windows.h>

#define _ENTRY_CALL_	WINAPI			//should be __stdcall
#define _MAIN_			WinMain
#define _ENTRY_ARGS_	HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int nCmdShow

typedef HWND ctx_handle_t;
typedef LPWSTR _WCHARSTR_;

typedef float	float_t;

#endif // #if USE( PLATFORM_WIN32 )

#if USE( DX12_API )

#include "DX\d3d12.h"
#include "DX\dxgi1_4.h"
#include "DX\d3dcompiler.h"
#include <DirectXMath.h>
#include "DX\d3dx12.h"

#include <shellapi.h>
#include <wrl.h>

#define DXERROR(x) if(FAILED(x)){ printf(#x); }
#define GFX_FRAMEBUFFER_FORMAT DXGI_FORMAT_R8G8B8A8_UNORM
#define GFX_DEPTH_STENCIL_FORMAT DXGI_FORMAT_D32_FLOAT // no stencil. not yet
#define GFX_DSV_ELEMENT_SIZE	4 // 32 bits. update this when GFX_DEPTH_STENCIL_FORMAT changes

#endif //#if USE( DX12_API )

