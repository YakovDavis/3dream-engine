#include "engine/systems/NavigationSystem.h"

#include "D3E/Components/TransformComponent.h"
#include "D3E/Components/navigation/NavigationAgentComponent.h"
#include "D3E/Components/navigation/NavmeshComponent.h"
#include "D3E/CrowdConfig.h"
#include "D3E/Debug.h"
#include "D3E/Game.h"
#include "DetourCommon.h"

using namespace D3E;

NavigationSystem::NavigationSystem(Game* g)
{
	game_ = g;
	crowdDebugInfo_ = {};
}

void NavigationSystem::Init()
{
}

void NavigationSystem::Update(entt::registry& reg, Game* game, float dT)
{
	auto navmeshView = reg.view<NavmeshComponent>();

	if (navmeshView.empty())
	{
		return;
	}

	auto nce = navmeshView.front();
	auto& nc = navmeshView.get<NavmeshComponent>(nce);

	auto nav = nc.navMesh;
	auto crowd = nc.crowd;

	crowd->update(dT, &crowdDebugInfo_);

	auto view = reg.view<NavigationAgentComponent, TransformComponent>();

	view.each([](NavigationAgentComponent& nc, TransformComponent& tc) {});
}

void NavigationSystem::Play(entt::registry& reg, Game* game)
{
	auto navMeshView = reg.view<NavmeshComponent>();
	auto ne = navMeshView.front();
	auto& nc = reg.get<NavmeshComponent>(ne);

	if (!nc.isBuilt)
	{
		if (!game_->BuildNavmesh(ne))
		{
			Debug::LogError(
				"[NavigationSystem] : Play(): navmesh was not built.");

			return;
		}
	}

	InitCrowd(nc);
	InitAgents(reg, nc);
}

void NavigationSystem::Pause(entt::registry& reg, Game* game)
{
}

void NavigationSystem::Stop(entt::registry& reg, Game* game)
{
}

void NavigationSystem::StopReset(entt::registry& reg, Game* game)
{
}

void NavigationSystem::InitCrowd(NavmeshComponent& nc)
{
	auto r = nc.crowd->init(10, nc.config.agentRadius, nc.navMesh);

	dtObstacleAvoidanceParams params;

	memcpy(&params, nc.crowd->getObstacleAvoidanceParams(0),
	       sizeof(dtObstacleAvoidanceParams));

	// Low (11)
	params.velBias = 0.5f;
	params.adaptiveDivs = 5;
	params.adaptiveRings = 2;
	params.adaptiveDepth = 1;
	nc.crowd->setObstacleAvoidanceParams(0, &params);

	// Medium (22)
	params.velBias = 0.5f;
	params.adaptiveDivs = 5;
	params.adaptiveRings = 2;
	params.adaptiveDepth = 2;
	nc.crowd->setObstacleAvoidanceParams(1, &params);

	// Good (45)
	params.velBias = 0.5f;
	params.adaptiveDivs = 7;
	params.adaptiveRings = 2;
	params.adaptiveDepth = 3;
	nc.crowd->setObstacleAvoidanceParams(2, &params);

	// High (66)
	params.velBias = 0.5f;
	params.adaptiveDivs = 7;
	params.adaptiveRings = 3;
	params.adaptiveDepth = 3;

	nc.crowd->setObstacleAvoidanceParams(3, &params);
}

void NavigationSystem::InitAgents(entt::registry& reg, NavmeshComponent& nc)
{
	auto crowd = nc.crowd;

	dtCrowdAgentParams ap;
	memset(&ap, 0, sizeof(ap));

	NavmeshConfig nConfig = nc.config;

	ap.radius = nConfig.agentRadius;
	ap.height = nConfig.agentHeight;
	ap.maxAcceleration = 8.0f;
	ap.maxSpeed = 3.5f;
	ap.collisionQueryRange = ap.radius * 12.0f;
	ap.pathOptimizationRange = ap.radius * 30.0f;
	ap.updateFlags = 0;

	CrowdConfig cc;

	if (cc.anticipateTurns)
		ap.updateFlags |= DT_CROWD_ANTICIPATE_TURNS;
	if (cc.optimizeVis)
		ap.updateFlags |= DT_CROWD_OPTIMIZE_VIS;
	if (cc.optimizeTopo)
		ap.updateFlags |= DT_CROWD_OPTIMIZE_TOPO;
	if (cc.obstacleAvoidance)
		ap.updateFlags |= DT_CROWD_OBSTACLE_AVOIDANCE;
	if (cc.separation)
		ap.updateFlags |= DT_CROWD_SEPARATION;
	ap.obstacleAvoidanceType = (unsigned char)cc.obstacleAvoidanceType;
	ap.separationWeight = cc.separationWeight;

	auto view = reg.view<NavigationAgentComponent, TransformComponent>();

	view.each(
		[&ap, crowd](NavigationAgentComponent& ac, TransformComponent& tc)
		{
			auto& posV = tc.position;
			float agentPos[3] = {posV.x, posV.y, posV.z};
			auto idx = crowd->addAgent(&agentPos[0], &ap);
			if (idx != -1)
			{
				ac.idx = idx;

				// Init trail
				AgentTrail* trail = &ac.trail;
				for (int i = 0; i < kMaxTrails; ++i)
					dtVcopy(&trail->trail[i * 3], &agentPos[0]);
				trail->htrail = 0;
			}
		});
}
