#include "EntryPoint.h"
#include "Game.h"

class CubeGame : public D3E::Game
{

};

D3E::App* D3E::CreateApp()
{
	return new CubeGame();
}