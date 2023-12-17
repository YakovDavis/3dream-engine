#pragma once

#include "D3E/EntryPoint.h"
#include "D3E/Game.h"

class ScriptingSandbox : public D3E::Game
{
	void Init() override;
	void Update(float deltaTime) override;
};

D3E::App* D3E::CreateApp()
{
	return new ScriptingSandbox();
}