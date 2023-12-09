#include "DuckGame.h"

#include "D3E/Components/UserScript.h"
#include "D3E/Debug.h"
#include "D3E/TimerHandle.h"
#include "D3E/TimerManager.h"
#include "D3E/engine/ConsoleManager.h"
#include "D3E/systems/CreationSystems.h"
#include "sol/sol.hpp"

#include <format>

void DuckGame::Update(float deltaTime)
{
	Game::Update(deltaTime);

	us_.Update(deltaTime);

	if (fireCounter_ > 2)
	{
		D3E::TimerManager::GetInstance().ClearTimer(handle2_);
	}
}

void DuckGame::TimerHandler()
{
	D3E::Debug::LogMessage("Member function timer fired!");
}

static void Handler()
{
	D3E::Debug::LogMessage("Static function timer fired!");
}

void DuckGame::Init()
{
	Game::Init();

	lua_.open_libraries(sol::lib::base);

	auto script = lua_.load_file("Scripts/myobject.lua");
	auto r = script();

	D3E::TransformComponent tc = {};
	D3E::ObjectInfoComponent info = {};
	info.name = "Player";

	tc.position = Vector3(0, 0, 0);

	D3E::CreationSystems::CreateDefaultPlayer(GetRegistry(), tc);

	tc.position = Vector3(0, 0, 50);
	tc.scale = Vector3(0.2f, 0.2f, 0.2f);
	info.name = "Cerberus";

	D3E::CreationSystems::CreateSM(GetRegistry(), info, tc,
	                               "60481bf4-cab2-4ad9-8d0d-95556bd20f7d");

	info.name = "DirectionalLight";

	D3E::CreationSystems::CreateLight(GetRegistry(), info, tc);
}
