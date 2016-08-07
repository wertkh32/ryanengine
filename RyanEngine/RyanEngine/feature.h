#pragma once
#define IN_USE 7
#define NOT_IN_USE 3
#define USE( x )	( ( IN_USE / ( x ) ) == 1 )

#define PLATFORM_WIN32	IN_USE
#define DX12_API		IN_USE

enum DISPLAY_RESOLUTIONS
{
	DRES_720p,
	DRES_900p,
	DRES_1080p
};


#define WINDOWED_WIDTH 1280
#define WINDOWED_HEIGHT 720
#define GAME_TITLE	"RYAN AND HIS GAY FRIENDS"
