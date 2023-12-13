#pragma once

#include "D3E/CommonHeader.h"
#include "D3E/EntryPoint.h"
#include "D3E/Game.h"
#include "D3E/TimerHandle.h"

class NewCubeGame : public D3E::Game
{
	void Init() override;
	void Update(float deltaTime) override;
};

D3E::App* D3E::CreateApp()
{
	return new NewCubeGame();
}
