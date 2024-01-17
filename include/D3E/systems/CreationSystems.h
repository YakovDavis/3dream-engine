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
		static entt::entity CreateDefaultEmpty(entt::registry& registry);
		static entt::entity CreateDefaultPlane(entt::registry& registry);
		static entt::entity CreateDefaultCube(entt::registry& registry);
		static entt::entity CreateDefaultSphere(entt::registry& registry);
		static entt::entity CreateDefaultLight(entt::registry& registry);

		static entt::entity CreatePhysicalCube(entt::registry& registry, const ObjectInfoComponent& info, const TransformComponent& tc, const PhysicsComponent& physc);
		static entt::entity CreateSM(entt::registry& registry, const ObjectInfoComponent& info, const TransformComponent& tc, const String& meshUuid, const String& materialUuid);
		static entt::entity CreateLight(entt::registry& registry, const ObjectInfoComponent& info, const TransformComponent& tc);
		static entt::entity CreateDefaultPlayer(entt::registry& registry, const TransformComponent& tc);
		static entt::entity CreateEditorDebugRender(entt::registry& registry);
		static entt::entity CreatePhysicalCharacter(entt::registry& registry, const ObjectInfoComponent& info, const TransformComponent& tc, const PhysicsCharacterComponent& character);
		static entt::entity CreatePurelyPhysicalObject(entt::registry& registry, const ObjectInfoComponent& info, const TransformComponent& tc, const PhysicsComponent& physc);
		static entt::entity OnCreateObjectButtonPressed(entt::registry& registry, int item);

		static void CreateDefaultFPSControllerComponent(entt::registry& registry, entt::entity& entity);
		static void CreateDefaultCameraComponent(entt::registry& registry, entt::entity& entity);
		static void CreateDefaultLightComponent(entt::registry& registry, entt::entity& entity);
		static void CreateDefaultStaticMeshComponent(entt::registry& registry, entt::entity& entity);
		static void CreateDefaultSoundComponent(entt::registry& registry, entt::entity& entity);
		static void CreateDefaultSoundListenerComponent(entt::registry& registry, entt::entity& entity);
		static void CreateDefaultScriptComponent(entt::registry& registry, entt::entity& entity);
		static void CreateDefaultNavigationComponent(entt::registry& registry, entt::entity& entity);
	};
}
