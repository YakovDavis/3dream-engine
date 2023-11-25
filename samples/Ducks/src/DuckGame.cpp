#include "DuckGame.h"

#include "D3E/systems/CreationSystems.h"
#include "D3E/engine/ConsoleManager.h"

void DuckGame::Update(float deltaTime)
{
	Game::Update(deltaTime);
}

void DuckGame::Init()
{
	Game::Init();

	D3E::TransformComponent tc = {};
	D3E::ObjectInfoComponent info = {};
	info.name = "Duck";

	tc.position = Vector3(0, 0, 10);

	D3E::CreationSystems::CreateDefaultPlayer(GetRegistry(), tc);

	tc.position = Vector3(0, 0, 0);

	D3E::CreationSystems::CreateSM(GetRegistry(), info, tc, "5bfb3a8a-46c0-4125-83cb-e031a263e91a");
}
