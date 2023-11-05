#include "EntryPoint.h"
#include "Game.h"

class CubeGame : public D3E::Game
{
	void Update(float deltaTime) override;
};

void CubeGame::Update(float deltaTime)
{
	Game::Update(deltaTime);
	//std::cout << "frame\n";
}

D3E::App* D3E::CreateApp()
{
	return new CubeGame();
}