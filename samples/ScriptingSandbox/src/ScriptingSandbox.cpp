#include "ScriptingSandbox.h"

#include "D3E/CommonHeader.h"
#include "D3E/Components/ScriptComponent.h"
#include "D3E/Components/navigation/NavmeshComponent.h"
#include "D3E/Debug.h"
#include "D3E/scripting/ScriptingEngine.h"
#include "D3E/systems/CreationSystems.h"

D3E_APP_CORE

void ScriptingSandbox::Init()
{
	Game::Init();

	D3E::TransformComponent tc = {};
	D3E::ObjectInfoComponent info = {};
	info.name = "Player";

	tc.position = Vector3(0, 0, 0);

	D3E::CreationSystems::CreateDefaultPlayer(GetRegistry(), tc);

	tc.position = Vector3(0, 0, 50);
	tc.scale = Vector3(0.2f, 0.2f, 0.2f);
	info.name = "Duck";

	auto duckId = D3E::CreationSystems::CreateSM(
		GetRegistry(), info, tc, "5bfb3a8a-46c0-4125-83cb-e031a263e91a",
		"ca626ac7-8f6d-44bd-9fbf-9a529fc577f4");

	info.internalObject = true;
	info.name = "InternalObject";
	tc.position = Vector3(20, 0, 50);
	D3E::CreationSystems::CreateSM(GetRegistry(), info, tc,
	                               "5bfb3a8a-46c0-4125-83cb-e031a263e91a",
	                               "ca626ac7-8f6d-44bd-9fbf-9a529fc577f4");

	D3E::String scriptUuid = "72d4a952-074c-4223-8ded-40c09c182061";
	auto scriptComponent = D3E::ScriptComponent(duckId, scriptUuid);

	GetRegistry().emplace<D3E::ScriptComponent>(duckId, scriptComponent);

	info.name = "Dungeon";
	tc.position = Vector3(0.f, 0.f, 0.f);
	tc.scale = Vector3(1.f, 1.f, 1.f);
	auto te = D3E::CreationSystems::CreateSM(
		GetRegistry(), info, tc, "51f14cf8-d75f-4819-9ee7-a487919740c4",
		"ca626ac7-8f6d-44bd-9fbf-9a529fc577f4");
	registry_.emplace<D3E::NavmeshComponent>(te, D3E::NavmeshComponent());

	info.name = "DirectionalLight";

	D3E::CreationSystems::CreateLight(GetRegistry(), info, tc);
}

void ScriptingSandbox::Update(float deltaTime)
{
	Game::Update(deltaTime);
}
