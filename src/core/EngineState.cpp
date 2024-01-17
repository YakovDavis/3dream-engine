#include "EngineState.h"
#include "D3E/Game.h"
#include "render/Display.h"
#include "D3E/Uuid.h"

D3E::Game* D3E::EngineState::game_ = nullptr;
bool D3E::EngineState::isViewportDirty = true;
int D3E::EngineState::viewportW = 0;
int D3E::EngineState::viewportH = 0;
bool D3E::EngineState::isInitialized = false;
entt::entity D3E::EngineState::currentPlayer = entt::null;
D3E::String D3E::EngineState::CurrentMapUuid = D3E::EmptyIdString;

int D3E::EngineState::GetViewportHeight()
{
	if (isViewportDirty)
	{
		RefreshViewport();
	}
	return viewportH;
}

int D3E::EngineState::GetViewportWidth()
{
	if (isViewportDirty)
	{
		RefreshViewport();
	}
	return viewportW;
}

void D3E::EngineState::RefreshViewport()
{
	if (!isInitialized)
	{
		return;
	}

	viewportW = game_->GetDisplay()->ClientWidth;
	viewportH = game_->GetDisplay()->ClientHeight;

	isViewportDirty = false;
}

void D3E::EngineState::Initialize(D3E::Game* game)
{
	game_ = game;
	isInitialized = true;
}
