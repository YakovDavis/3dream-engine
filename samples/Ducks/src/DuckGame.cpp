#include "DuckGame.h"

#include "D3E/Debug.h"
#include "D3E/TimerHandle.h"
#include "D3E/TimerManager.h"
#include "D3E/engine/ConsoleManager.h"
#include "D3E/systems/CreationSystems.h"

#include <format>

void DuckGame::Update(float deltaTime)
{
	Game::Update(deltaTime);

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

	// Lambda example
	D3E::TimerManager::GetInstance().SetTimer(
		handle_, [this]() { D3E::Debug::LogMessage("1 second timer fired!"); },
		1000);

	// Looping timer example
	D3E::TimerManager::GetInstance().SetTimer(
		handle2_,
		[this]()
		{
			D3E::Debug::LogMessage("3 seconds looping timer fired!");
			++fireCounter_;
		},
		3000, true);

	// Member function delegate example
	D3E::TimerManager::GetInstance().SetTimer(handle3_, this,
	                                          &DuckGame::TimerHandler, 10000);
	// Static function delegate example
	D3E::TimerManager::GetInstance().SetTimer(handle4_, &Handler, 11000);

	// Timer for next tick example
	handle5_ = D3E::TimerManager::GetInstance().SetTimerForNextTick(
		[]() { D3E::Debug::LogMessage("Next tick timer fired!"); });

	D3E::TransformComponent tc = {};
	D3E::ObjectInfoComponent info = {};
	info.name = "Player";

	tc.position = Vector3(0, 0, 0);

	D3E::CreationSystems::CreateDefaultPlayer(GetRegistry(), tc);

	tc.position = Vector3(0, 0, 50);
	tc.scale = Vector3(0.2f, 0.2f, 0.2f);
	info.name = "Cerberus1";

	D3E::CreationSystems::CreateSM(GetRegistry(), info, tc, "60481bf4-cab2-4ad9-8d0d-95556bd20f7d");

	info.name = "Cerberus2";
	tc.position = Vector3(50, 0, 0);
	D3E::CreationSystems::CreateSM(GetRegistry(), info, tc, "60481bf4-cab2-4ad9-8d0d-95556bd20f7d");

	info.name = "Cerberus3";
	tc.position = Vector3(-50, 0, 0);
	D3E::CreationSystems::CreateSM(GetRegistry(), info, tc, "60481bf4-cab2-4ad9-8d0d-95556bd20f7d");

	info.name = "DirectionalLight";

	D3E::CreationSystems::CreateLight(GetRegistry(), info, tc);
}
