#include "universal.h"
#include "game_init_win32.h"

int _ENTRY_CALL_ _MAIN_( _ENTRY_ARGS_ )
{

#if USE( PLATFORM_WIN32 )
	return Init_Window( Game_Engine::Instance(), hInstance, nCmdShow );
#else // #if USE( PLATFORM_WIN32 )
	return 0;
#endif // #else // #if USE( PLATFORM_WIN32 )
}