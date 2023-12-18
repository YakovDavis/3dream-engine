#pragma once

#include "D3E/EntryPoint.h"
#include "D3E/Game.h"
#include "D3E/TimerHandle.h"

class CharacterGame : public D3E::Game
{
	void Init() override;
	void Update(float deltaTime) override;
	void TimerHandler();

	D3E::TimerHandle handle_;
	D3E::TimerHandle handle2_;
	D3E::TimerHandle handle3_;
	D3E::TimerHandle handle4_;
	D3E::TimerHandle handle5_;
	int fireCounter_ = 0;
};

D3E::App* D3E::CreateApp()
{
	return new CharacterGame();
}
