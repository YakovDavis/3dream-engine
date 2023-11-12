#include "D3E/systems/CreationSystems.h"

#include "engine/components/ObjectInfoComponent.h"
#include "D3E/components/TransformComponent.h"
#include "D3E/components/render/StaticMeshComponent.h"

entt::entity D3E::CreationSystems::CreateCubeSM(entt::registry& registry)
{
	const auto e = registry.create();
	StaticMeshComponent sm;
	sm.meshName = "Cube";
	sm.pipelineName = "SimpleForward";

	ObjectInfoComponent info;
	info.name = "Cube";

	TransformComponent transform;
	transform.position_ = {0, 1, 0};
	transform.rotation_ = {0, 0, 0, 1};
	transform.scale_ = {1, 1, 1};

	registry.emplace<ObjectInfoComponent>(e, info);
	registry.emplace<TransformComponent>(e, transform);
	registry.emplace<StaticMeshComponent>(e, sm);
	return e;
}
