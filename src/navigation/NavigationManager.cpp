#include "NavigationManager.h"

#include "D3E/Components/navigation/NavigationAgentComponent.h"
#include "D3E/Components/navigation/NavmeshComponent.h"
#include "entt/entt.hpp"

using namespace D3E;

NavigationManager& NavigationManager::GetInstance()
{
	static NavigationManager instance;

	return instance;
}

void NavigationManager::Init(Game* g)
{
	game_ = g;
}

void NavigationManager::MoveTo(entt::entity e,
                               const DirectX::SimpleMath::Vector3& pos)
{
	GetInstance().MoveToInternal(e, pos);
}

void NavigationManager::CancelTarget(entt::entity e)
{
	GetInstance().CancelTargetInternal(e);
}

NavigationManager::NavigationManager() : game_(nullptr)
{
}

void NavigationManager::MoveToInternal(entt::entity e,
                                       const DirectX::SimpleMath::Vector3& pos)
{
	auto& reg = game_->GetRegistry();

	auto navmeshView = reg.view<NavmeshComponent>();
	auto navmeshEntity = navmeshView.front();
	auto& navmesh = navmeshView.get<NavmeshComponent>(navmeshEntity);

	auto& agent = reg.get<NavigationAgentComponent>(e);

	auto navQuery = navmesh.navQuery;
	auto crowd = navmesh.crowd;

	const auto filter = crowd->getFilter(0);
	const auto halfExtents = crowd->getQueryExtents();
	
	const float extents[] = {10, 10, 10};

	float point[3] = {pos.x, pos.y, pos.z};
	navQuery->findNearestPoly(point, extents, filter, &agent.targetRef,
	                          agent.targetPosition);

	const dtCrowdAgent* ag = crowd->getAgent(agent.idx);

	if (!ag)
	{
		return;
	}

	if (ag->active)
	{
		crowd->requestMoveTarget(agent.idx, agent.targetRef,
		                         agent.targetPosition);
	}
}

void NavigationManager::CancelTargetInternal(entt::entity e)
{
	auto& reg = game_->GetRegistry();

	auto navmeshView = reg.view<NavmeshComponent>();
	auto navmeshEntity = navmeshView.front();
	auto& navmesh = navmeshView.get<NavmeshComponent>(navmeshEntity);
	auto crowd = navmesh.crowd;

	auto& nac = reg.get<NavigationAgentComponent>(e);

	crowd->resetMoveTarget(nac.idx);

	auto agent = crowd->getAgent(nac.idx);
	nac.targetRef = agent->targetRef;

	memset(nac.targetPosition, 0, std::size(nac.targetPosition));
}