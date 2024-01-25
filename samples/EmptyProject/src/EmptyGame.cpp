#include "EmptyGame.h"

#include "D3E/systems/CreationSystems.h"

D3E_APP_CORE

void EmptyGame::Init()
{
	Game::Init();

	D3E::TransformComponent tc = {};
	D3E::ObjectInfoComponent info = {};

	info.name = "Player";
	D3E::CreationSystems::CreateDefaultPlayer(GetRegistry(), tc);

	info.name = "DirectionalLight";
	D3E::CreationSystems::CreateLight(GetRegistry(), info, tc);

	D3E::CreationSystems::CreateSkybox(GetRegistry());
}
