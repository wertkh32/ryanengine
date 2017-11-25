#include "game_engine.h"

Game_Engine::Game_Engine() : renderer()
{
}

void Game_Engine::ParseArgs(_WCHARSTR_ * argv, int arg_count)
{

}

void Game_Engine::Init( ctx_handle_t contextHandle)
{
	memset ( &renderer, 0, sizeof ( renderer ) );
	renderer.Init(contextHandle);
}

void Game_Engine::FrameCallback()
{
	Render();
}

void Game_Engine::Render()
{
	renderer.Render();
}

void Game_Engine::OnKeyDown(uchar key)
{
}

void Game_Engine::OnKeyUp(uchar key)
{
}

void Game_Engine::OnDestroy()
{
}

Game_Engine::~Game_Engine()
{
}
