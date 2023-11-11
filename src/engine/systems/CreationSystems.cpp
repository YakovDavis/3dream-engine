#include "D3E/systems/CreationSystems.h"

#include "D3E/components/render/StaticMeshComponent.h"
#include "render/GeometryGenerator.h"

entt::entity D3E::CreationSystems::CreateCubeSM(entt::registry& registry)
{
	const auto e = registry.create();
	StaticMeshComponent sm = {};
	//GeometryGenerator::CreateBox(sm, 1.0f, 1.0f, 1.0f, 0);
	sm.pipelineName = "SimpleForward";
	registry.emplace<StaticMeshComponent>(e, sm);
	return e;
}
