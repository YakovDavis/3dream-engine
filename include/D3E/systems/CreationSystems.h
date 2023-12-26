#pragma once

#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Components/TransformComponent.h"
#include "D3E/Components/PhysicsComponent.h"
#include "D3E/Components/PhysicsCharacterComponent.h"
#include "entt/entt.hpp"

namespace D3E
{
	class CreationSystems
	{
	public:
		static entt::entity CreateCubeSM(entt::registry& registry, const ObjectInfoComponent& info, const TransformComponent& tc);
		static entt::entity CreatePhysicalCube(entt::registry& registry, const ObjectInfoComponent& info, const TransformComponent& tc, const PhysicsComponent& physc);
		static entt::entity CreateSM(entt::registry& registry, const ObjectInfoComponent& info, const TransformComponent& tc, const String& meshUuid, const String& materialUuid);
		static entt::entity CreateLight(entt::registry& registry, const ObjectInfoComponent& info, const TransformComponent& tc);
		static entt::entity CreateDefaultPlayer(entt::registry& registry, const TransformComponent& tc);
		static entt::entity CreateEditorDebugRender(entt::registry& registry);
		static entt::entity CreatePhysicalCharacter(entt::registry& registry, const ObjectInfoComponent& info, const TransformComponent& tc, const PhysicsCharacterComponent& character);
		static entt::entity CreateCharacterWithoutCamera(entt::registry& registry, const ObjectInfoComponent& info, const TransformComponent& tc, const PhysicsCharacterComponent& character);
		static entt::entity CreatePurelyPhysicalObject(entt::registry& registry, const ObjectInfoComponent& info, const TransformComponent& tc, const PhysicsComponent& physc);
	};
}
