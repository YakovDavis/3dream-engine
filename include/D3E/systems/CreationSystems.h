#pragma once

#include "D3E/Components/TransformComponent.h"
#include "engine/components/ObjectInfoComponent.h"
#include "entt/entt.hpp"

namespace D3E
{
	class CreationSystems
	{
	public:
		static entt::entity CreateCubeSM(entt::registry& registry, const ObjectInfoComponent& info, const TransformComponent& tc);
		static entt::entity CreateDefaultPlayer(entt::registry& registry, const TransformComponent& tc);
	};
}
