#include "D3E/EntryPoint.h"
#include "D3E/Game.h"

class CubeGame : public D3E::Game
{
	void Init() override;
	void Update(float deltaTime) override;
};

void CubeGame::Update(float deltaTime)
{
	Game::Update(deltaTime);
}

void CubeGame::Init()
{
	Game::Init();

}

D3E::App* D3E::CreateApp()
{
	return new CubeGame();
}