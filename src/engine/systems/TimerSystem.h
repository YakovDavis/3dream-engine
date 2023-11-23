#pragma once

#include "D3E/systems/GameSystem.h"

namespace D3E
{
	class TimerSystem : public GameSystem
	{
	public:
		void Update(entt::registry& reg, Game* game, float dT)override;
	};
} // namespace D3E