#pragma once

#include "entt/entt.hpp"

namespace D3E
{
	class Game;

	class PerTickSystem
	{
	public:
		virtual void Run(entt::registry& reg, Game* game, float dT) = 0;
	};
}
