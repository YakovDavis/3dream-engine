#pragma once

#include "D3E/Components/PhysicsComponent.h"
#include "D3E/Components/ScriptComponent.h"
#include "D3E/Components/TransformComponent.h"
#include "D3E/Debug.h"
#include "EASTL/vector.h"
#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/BodyID.h"
#include "entt/entt.hpp"
#include "sol/sol.hpp"

#include <EASTL/string.h>
#include <optional>

namespace D3E
{
	class Game;

	enum class ComponentType
	{
		kTransformComponent = 0,
		kPhysicsComponent,
		kScriptComponent,
		kPhysicsCharacterComponent,
		kObjectInfoComponent,
		kSoundComponent,
		kLightComponent,
		kCameraComponent,
		kStaticMeshComponent,
	};

	class ECSUtils
	{
	private:
		static Game* game_;

	public:
		ECSUtils() = delete;

		static void Init(Game* g);

		static std::optional<entt::entity>
		GetEntityByBodyId(entt::registry& r, const JPH::BodyID& bodyId);

		static std::optional<entt::entity>
		GetEntityWithTag(entt::registry& r, const eastl::string& tag);

		static eastl::vector<entt::entity>
		GetEntitiesWithTag(entt::registry& r, const eastl::string& tag);

		static ScriptComponent* GetScriptComponent(entt::registry& r,
		                                           entt::entity e,
		                                           const String& className);

		static void DestroyEntity(entt::registry& r, entt::entity e);

		static void DestroyEntities(entt::registry& r,
		                           const eastl::vector<entt::entity>& entities);

		static eastl::vector<entt::entity>
		GetEntitiesWithScript(entt::registry& r, const String& className);

		template<typename T>
		static T& Get(entt::registry& r, const entt::entity e)
		{
			return r.get<T>(e);
		}

		template<typename T>
		static T* TryGet(entt::registry& r, const entt::entity e)
		{
			return r.try_get<T>(e);
		}

		template<typename T>
		static std::optional<entt::entity>
		GetEntityByBodyId(entt::registry& r, const JPH::BodyID& bodyId)
		{
			auto view = r.view<T>();

			for (auto [e, pc] : view.each())
			{
				if (pc.bodyID_ == bodyId)
				{
					return e;
				}
			}

			return std::nullopt;
		}
	};
} // namespace D3E