#pragma once
#include "universal.h"
#include "game_engine.h"

#if USE( PLATFORM_WIN32 )

int Init_Window( Game_Engine* engine, HINSTANCE hInstance, int nCmdShow );

#endif // #if USE( PLATFORM_WIN32 )