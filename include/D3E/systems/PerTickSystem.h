#pragma once

#include "entt/entt.hpp"

namespace D3E
{
	class PerTickSystem
	{
	public:
		virtual void Run(entt::registry& reg, float dT) = 0;
	};
}
