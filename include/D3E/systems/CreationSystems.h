#pragma once

#include "entt/entt.hpp"

namespace D3E
{
	class CreationSystems
	{
	public:
		static entt::entity CreateCubeSM(entt::registry& registry);
		static entt::entity CreateDefaultPlayer(entt::registry& registry);
	};
}
