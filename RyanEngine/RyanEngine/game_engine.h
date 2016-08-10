#pragma once
#include "universal.h"
class Game_Engine
{
private:
	Game_Engine();
public:
	static Game_Engine* Instance()
	{
		static Game_Engine* engine = 0;
		// instantiate only once... no more
		if( !engine )
			engine = new Game_Engine();
		return engine;
	}

	void ParseArgs( _WCHARSTR_* argv, int arg_count );
	void InitRenderer( _CTX_HANDLE_ contextHandle );
	
	void FrameCallback();
	void Render();

	void OnKeyDown(uchar key);
	void OnKeyUp(uchar key);
	void OnDestroy();

	~Game_Engine();
};

