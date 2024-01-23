#include "ChildTransformSynchronizationSystem.h"

#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Components/TransformComponent.h"
#include "D3E/Debug.h"
#include "D3E/Game.h"
#include "SimpleMath.h"

using namespace D3E;
using namespace DirectX::SimpleMath;

static void CalcTransform(TransformComponent& tc, const TransformComponent& parentTc)
{
	auto parentMatrix = DirectX::SimpleMath::Matrix::CreateScale(parentTc.scale) *
	                    DirectX::SimpleMath::Matrix::CreateFromQuaternion(parentTc.rotation) *
	                    DirectX::SimpleMath::Matrix::CreateTranslation(parentTc.position);
	auto relChildMatrix = DirectX::SimpleMath::Matrix::CreateScale(tc.relativeScale) *
	                   DirectX::SimpleMath::Matrix::CreateFromQuaternion(tc.relativeRotation) *
	                   DirectX::SimpleMath::Matrix::CreateTranslation(tc.relativePosition);

	auto res = relChildMatrix * parentMatrix;

	res.Decompose(tc.scale, tc.rotation, tc.position);
}

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

			if (info.parentId != EmptyIdString)
			{
				entt::entity parent;
				if (game->FindEntityByID(parent, info.parentId))
				{
					auto parentTc = reg.try_get<TransformComponent>(parent);
					if (parentTc)
					{
						reg.patch<TransformComponent>(
							entity,
							[&](auto& ct)
							{
								CalcTransform(ct, *parentTc);
							});
					}
				}
			}

			if (childEntities_.find(info.id) == childEntities_.end())
			{
				return;
			}

			const auto& parentTransform = reg.get<TransformComponent>(entity);

			for (auto& ce : childEntities_[info.id])
			{
				auto& childTransform = reg.get<TransformComponent>(ce);

				reg.patch<TransformComponent>(
					ce,
					[&](auto& ct)
					{
						CalcTransform(ct, parentTransform);
					});
			}
		});
}

void ChildTransformSynchronizationSystem::TransformCreatedHandler(
	entt::registry& registry, entt::entity entity)
{
	const auto& objectInfo = registry.get<ObjectInfoComponent>(entity);

	if (objectInfo.parentId != EmptyIdString)
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

	if (objectInfo.parentId != EmptyIdString)
	{
		childEntities_[objectInfo.parentId].erase(entity);
	}
}

void ChildTransformSynchronizationSystem::OnParentUpdate(entt::registry& registry,
	entt::entity e, const String& prevParent)
{
	auto info = registry.try_get<ObjectInfoComponent>(e);
	if (!info)
	{
		return;
	}
	if (prevParent != EmptyIdString)
	{
		if (childEntities_.find(info->parentId) != childEntities_.end())
		{
			childEntities_[info->parentId].erase(e);
		}
	}
	if (info->parentId != EmptyIdString)
	{
		if (childEntities_.find(info->parentId) == childEntities_.end())
		{
			childEntities_.insert(info->parentId);
		}

		childEntities_.at(info->parentId).insert(e);
	}
}
