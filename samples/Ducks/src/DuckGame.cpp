#include "DuckGame.h"

#include "D3E/Components/TransformComponent.h"
#include "D3E/Debug.h"
#include "D3E/TimerHandle.h"
#include "D3E/TimerManager.h"
#include "D3E/engine/ConsoleManager.h"
#include "D3E/systems/CreationSystems.h"
#include "SimpleMath.h"

#include <format>

D3E_APP_CORE

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

	//D3E::CreationSystems::CreateSM(
	//	GetRegistry(), info, tc, "60481bf4-cab2-4ad9-8d0d-95556bd20f7d", "ca626ac7-8f6d-44bd-9fbf-9a529fc577f4");

	info.name = "Cerberus2";
	tc.position = Vector3(20, 0, 0);
	auto cer2 = D3E::CreationSystems::CreateSM(GetRegistry(), info, tc, "60481bf4-cab2-4ad9-8d0d-95556bd20f7d", "ca626ac7-8f6d-44bd-9fbf-9a529fc577f4");

	info.name = "Cerberus3";
	//info.parentId = registry_.get<D3E::ObjectInfoComponent>(cer2).id;
	tc.position = Vector3(-20, 0, 0);
	//tc.relativePosition = Vector3(-20, 0, 0);
	D3E::CreationSystems::CreateSM(GetRegistry(), info, tc, "60481bf4-cab2-4ad9-8d0d-95556bd20f7d", "ca626ac7-8f6d-44bd-9fbf-9a529fc577f4");

	info.name = "Floor";
	info.parentId = D3E::EmptyIdString;
	tc.position = Vector3(-50, -10, 50);
	tc.rotation = Quaternion::CreateFromAxisAngle(Vector3(1, 0, 0), DirectX::XM_PI / 2.0f);
	tc.scale = Vector3(200, 200, 200);
	D3E::CreationSystems::CreateSM(GetRegistry(), info, tc, D3E::kPlaneUUID, D3E::kDefaultGridMaterialUUID);

	info.name = "DirectionalLight";

	D3E::CreationSystems::CreateLight(GetRegistry(), info, tc);

	D3E::CreationSystems::CreateSkybox(GetRegistry());
}
