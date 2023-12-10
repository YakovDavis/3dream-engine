#include "InputSyncSystem.h"

#include "D3E/Components/MouseComponent.h"
#include "D3E/Game.h"
#include "input/InputDevice.h"

#include <iostream>

void D3E::InputSyncSystem::Update(entt::registry& reg, D3E::Game* game,
                                  float dT)
{
	auto view = reg.view<MouseComponent>();

	view.each([game](auto& mc)
	          {
				  mc.position = game->GetInputDevice()->MousePosition;
				  mc.delta = game->GetInputDevice()->MouseOffsetInTick;
			  });
}
