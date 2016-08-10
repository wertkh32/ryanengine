#pragma once
#include "feature.h"

#if USE( PLATFORM_WIN32 )

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

#include <windows.h>

#define _ENTRY_CALL_	WINAPI			//should be __stdcall
#define _MAIN_			WinMain
#define _ENTRY_ARGS_	HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int nCmdShow

#endif // #if USE( WIN32 )

#if USE( DX12_API )

#include "DX\d3d12.h"
#include "DX\dxgi1_4.h"
#include "DX\d3dcompiler.h"
#include <DirectXMath.h>
#include "DX\d3dx12.h"

#include <shellapi.h>
#include <wrl.h>

#endif //#if USE( DX12_API )