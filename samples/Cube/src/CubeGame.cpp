#include "CubeGame.h"

#include "D3E/systems/CreationSystems.h"

void CubeGame::Update(float deltaTime)
{
	Game::Update(deltaTime);
}

void CubeGame::Init()
{
	Game::Init();

	LoadTexture("wood", "wood.png");

	D3E::CreationSystems::CreateDefaultPlayer(GetRegistry());

	D3E::CreationSystems::CreateCubeSM(GetRegistry());
}
