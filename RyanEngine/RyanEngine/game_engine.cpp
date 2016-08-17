#include "game_engine.h"

Game_Engine::Game_Engine() : renderer()
{
}

void Game_Engine::ParseArgs(_WCHARSTR_ * argv, int arg_count)
{

}

void Game_Engine::Init( _CTX_HANDLE_ contextHandle)
{
	renderer.InitRenderer(contextHandle);
}

void Game_Engine::FrameCallback()
{
	Render();
}

void Game_Engine::Render()
{
	renderer.DrawFrame();
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
