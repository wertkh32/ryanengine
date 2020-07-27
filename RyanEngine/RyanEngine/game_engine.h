#pragma once
#include "universal.h"
#include "platform.h"
//#include "game_renderer.h"
#include "gfx_backend.h"

class Game_Engine
{
private:
	Game_Engine();
	GfxGameRenderer	renderer;
public:
	static Game_Engine* Instance()
	{
		static ALIGN( 16 ) byte inplaceMem[sizeof( Game_Engine )];
		static Game_Engine* engine = 0;
		// instantiate only once... no more
		if( !engine )
		{
			void *mem = reinterpret_cast< void* >( inplaceMem );
			engine = new ( mem ) Game_Engine();
		}
		return engine;
	}

	void ParseArgs( _WCHARSTR_* argv, int arg_count );
	void Init( ctx_handle_t contextHandle );
	
	void FrameCallback();
	void Render();

	void OnKeyDown(uchar key);
	void OnKeyUp(uchar key);
	void OnDestroy();

	~Game_Engine();
};

