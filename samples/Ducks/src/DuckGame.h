#pragma once

#include "D3E/EntryPoint.h"
#include "D3E/Game.h"
#include "D3E/TimerHandle.h"
#include "sol/sol.hpp"

class DuckGame : public D3E::Game
{
	void Init() override;
	void Update(float deltaTime) override;
	void TimerHandler();

	D3E::TimerHandle handle_;
	D3E::TimerHandle handle2_;
	D3E::TimerHandle handle3_;
	D3E::TimerHandle handle4_;
	D3E::TimerHandle handle5_;
	D3E::TimerHandle handle6_;
	int fireCounter_ = 0;
	sol::state lua_;
};

D3E::App* D3E::CreateApp()
{
	return new DuckGame();
}
