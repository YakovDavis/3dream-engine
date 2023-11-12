#pragma once

#include "D3E/systems/PerTickSystem.h"

namespace D3E
{
	class FPSControllerSystem : public PerTickSystem
	{
	public:
		void Run(entt::registry& reg, Game* game, float dT) override;
	};
}
