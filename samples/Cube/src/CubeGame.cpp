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

	D3E::TransformComponent tc = {};
	D3E::ObjectInfoComponent info = {};
	info.name = "Cube";

	tc.position = Vector3(0, 0, 10);

	D3E::CreationSystems::CreateDefaultPlayer(GetRegistry(), tc);

	tc.position = Vector3(0, 0, 0);

	D3E::CreationSystems::CreateCubeSM(GetRegistry(), info, tc);
}
