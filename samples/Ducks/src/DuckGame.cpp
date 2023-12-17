#include "DuckGame.h"

#include "D3E/Components/TransformComponent.h"
#include "D3E/Debug.h"
#include "D3E/TimerHandle.h"
#include "D3E/TimerManager.h"
#include "D3E/engine/ConsoleManager.h"
#include "D3E/systems/CreationSystems.h"
#include "SimpleMath.h"

#include <format>

void NewCubeGame::Update(float deltaTime)
{
	Game::Update(deltaTime);
}

void NewCubeGame::Init()
{
	Game::Init();

	D3E::TransformComponent tc = {};
	D3E::ObjectInfoComponent info = {};
	info.name = "Player";

	tc.position = Vector3(0, 0, 0);

	D3E::CreationSystems::CreateDefaultPlayer(GetRegistry(), tc);

	tc.position = Vector3(0, 0, 50);
	tc.scale = Vector3(0.2f, 0.2f, 0.2f);
	info.name = "Cerberus1";

	D3E::CreationSystems::CreateSM(
		GetRegistry(), info, tc, "60481bf4-cab2-4ad9-8d0d-95556bd20f7d");

	info.name = "Cerberus2";
	tc.position = Vector3(50, 0, 0);
	D3E::CreationSystems::CreateSM(GetRegistry(), info, tc, "60481bf4-cab2-4ad9-8d0d-95556bd20f7d");

	info.name = "Cerberus3";
	tc.position = Vector3(-50, 0, 0);
	D3E::CreationSystems::CreateSM(GetRegistry(), info, tc, "60481bf4-cab2-4ad9-8d0d-95556bd20f7d");

	info.name = "DirectionalLight";

	D3E::CreationSystems::CreateLight(GetRegistry(), info, tc);
}
