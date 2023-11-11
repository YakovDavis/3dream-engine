#pragma once

#include "entt/entt.hpp"

namespace D3E
{
	class PerRenderTickSystem
	{
	public:
		virtual void Render(entt::registry& reg) = 0;
	};
}
