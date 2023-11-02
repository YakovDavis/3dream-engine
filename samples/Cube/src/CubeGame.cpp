#include "EntryPoint.h"
#include "Game.h"

class CubeGame : public D3E::Game
{
	void Update(float deltaTime) override;
};

void CubeGame::Update(float deltaTime)
{
	Game::Update(deltaTime);
}

D3E::App* D3E::CreateApp()
{
	return new CubeGame();
}