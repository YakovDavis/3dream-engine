#include "ChildTransformSynchronizationSystem.h"

#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Components/TransformComponent.h"
#include "D3E/Debug.h"
#include "SimpleMath.h"

using namespace D3E;
using namespace DirectX::SimpleMath;

ChildTransformSynchronizationSystem::ChildTransformSynchronizationSystem(
	entt::registry& registry)
	: transformUpdateObserver_{registry,
                               entt::collector.update<TransformComponent>()}
{
	registry.on_construct<TransformComponent>()
		.connect<&ChildTransformSynchronizationSystem::TransformCreatedHandler>(
			this);

	registry.on_destroy<TransformComponent>()
		.connect<
			&ChildTransformSynchronizationSystem::TransformDestroyedHandler>(
			this);
}

void ChildTransformSynchronizationSystem::Update(entt::registry& reg,
                                                 Game* game, float dT)
{
	transformUpdateObserver_.each(
		[&](const auto entity)
		{
			auto& info = reg.get<ObjectInfoComponent>(entity);

			if (info.parentId != EmptyId)
				return;

			if (childEntities_.find(info.id) == childEntities_.end())
				return;

			const auto& parentTransform = reg.get<TransformComponent>(entity);

			for (auto& ce : childEntities_[info.id])
			{
				auto& childTransform = reg.get<TransformComponent>(ce);

				reg.patch<TransformComponent>(
					ce,
					[&](auto& ct)
					{
						ct.position =
							parentTransform.position + ct.relativePosition;

						ct.rotation =
							parentTransform.rotation * ct.relativePosition;

						ct.scale = parentTransform.scale * ct.relativeScale;
					});
			}
		});
}

void ChildTransformSynchronizationSystem::TransformCreatedHandler(
	entt::registry& registry, entt::entity entity)
{
	const auto& objectInfo = registry.get<ObjectInfoComponent>(entity);

	if (objectInfo.parentId != EmptyId)
	{
		if (childEntities_.find(objectInfo.parentId) == childEntities_.end())
		{
			childEntities_.insert(objectInfo.parentId);
		}

		childEntities_.at(objectInfo.parentId).insert(entity);
	}
}

void ChildTransformSynchronizationSystem::TransformDestroyedHandler(
	entt::registry& registry, entt::entity entity)
{
	const auto& objectInfo = registry.get<ObjectInfoComponent>(entity);

	if (objectInfo.parentId != EmptyId)
	{
		childEntities_[objectInfo.parentId].erase(entity);
	}
}
