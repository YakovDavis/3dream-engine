#pragma once

#include "D3E/systems/GameSystem.h"

namespace D3E
{
	class EditorPickingSystem : public GameSystem
	{
	public:
		void RunOnce(entt::registry& reg, Game* game, float dT) override;
	};
}
