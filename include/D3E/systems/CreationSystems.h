#pragma once

#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Components/TransformComponent.h"
#include "entt/entt.hpp"

namespace D3E
{
	class CreationSystems
	{
	public:
		static entt::entity CreateCubeSM(entt::registry& registry, const ObjectInfoComponent& info, const TransformComponent& tc);
		static entt::entity CreateSM(entt::registry& registry, const ObjectInfoComponent& info, const TransformComponent& tc, String meshUuid);
		static entt::entity CreateLight(entt::registry& registry, const ObjectInfoComponent& info, const TransformComponent& tc);
		static entt::entity CreateDefaultPlayer(entt::registry& registry, const TransformComponent& tc);
		static entt::entity CreateEditorDebugRender(entt::registry& registry);
	};
}
