#include "AiManagementSystem.h"

#include "D3E/Components/AiAgentComponent.h"

using namespace D3E;

AiManagementSystem::AiManagementSystem()
{
}

void AiManagementSystem::Update(entt::registry& reg, Game* game, float dT)
{
	auto view = reg.view<AiAgentComponent>();

	if (view.begin() == view.end())
	{
		return;
	}

	view.each([&dT](AiAgentComponent& ac) { ac.Update(); });
}

void AiManagementSystem::Play(entt::registry& reg, Game* game)
{
}

void AiManagementSystem::Pause(entt::registry& reg, Game* game)
{
}

void AiManagementSystem::Stop(entt::registry& reg, Game* game)
{
}