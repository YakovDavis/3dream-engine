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

void DuckGame::Init()
{
	Game::Init();

	D3E::TimerManager::GetInstance().SetTimer(
		handle_,
		[this]()
		{
			D3E::Debug::LogMessage("1 second not looping timer fired!");			
		},
		1000);

	D3E::TimerManager::GetInstance().SetTimer(
		handle2_,
		[this]()
		{
			D3E::Debug::LogMessage("3 seconds looping timer fired!");
			++fireCounter_;
		},
		3000, true);
	D3E::TimerManager::GetInstance().SetTimer(
		handle3_,
		[this]()
		{
			D3E::Debug::LogMessage("5 seconds not looping timer fired!");
		},
		5000);

	D3E::TransformComponent tc = {};
	D3E::ObjectInfoComponent info = {};
	info.name = "Duck";

	tc.position = Vector3(0, 0, 10);

	D3E::CreationSystems::CreateDefaultPlayer(GetRegistry(), tc);

	tc.position = Vector3(0, 0, 0);

	D3E::CreationSystems::CreateSM(GetRegistry(), info, tc,
	                               "5bfb3a8a-46c0-4125-83cb-e031a263e91a");
}
