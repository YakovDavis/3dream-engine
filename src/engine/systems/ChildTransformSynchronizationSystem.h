#pragma once

#include "D3E/Uuid.h"
#include "D3E/systems/GameSystem.h"
#include "EASTL/unordered_map.h"
#include "EASTL/unordered_set.h"

#include <entt/entt.hpp>

namespace D3E
{
	class ChildTransformSynchronizationSystem : public GameSystem
	{
	public:
		explicit ChildTransformSynchronizationSystem(entt::registry& registry);
		void Update(entt::registry& reg, Game* game, float dT) override;

	private:
		void TransformCreatedHandler(entt::registry& registry,
		                             entt::entity entity);
		void TransformDestroyedHandler(entt::registry& registry,
		                               entt::entity entity);

		entt::observer transformUpdateObserver_;
		eastl::unordered_map<Uuid, eastl::unordered_set<entt::entity>,
		                     D3E::UuidHash>
			childEntities_;
	};
} // namespace D3E