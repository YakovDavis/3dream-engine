#include "AiManagementSystem.h"

#include "D3E/Components/AiAgentComponent.h"

using namespace D3E;

AiManagementSystem::AiManagementSystem() : isStopped_(true), isPaused_(false)
{
}

void AiManagementSystem::Update(entt::registry& reg, Game* game, float dT)
{
	if (isPaused_ || isStopped_)
	{
		return;
	}

	auto view = reg.view<AiAgentComponent>();

	if (view.begin() == view.end())
	{
		return;
	}

	view.each([&dT](AiAgentComponent& ac) { ac.Update(); });
}

void AiManagementSystem::Play(entt::registry& reg, Game* game)
{
	isStopped_ = false;
	isPaused_ = false;
}

void AiManagementSystem::Pause(entt::registry& reg, Game* game)
{
	isStopped_ = false;
	isPaused_ = true;
}

void AiManagementSystem::Stop(entt::registry& reg, Game* game)
{
	isStopped_ = true;
	isPaused_ = false;
}