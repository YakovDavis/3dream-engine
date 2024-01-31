#include "D3E/Components/AiAgentComponent.h"

#include "D3E/CommonHeader.h"
#include "json.hpp"

using namespace D3E;

AiAgentComponent::AiAgentComponent() : agent("DefaultName"), fsm()
{
	CreateIdle();
	CreateMoveTo();
	CreatePerform();

	fsm.Push(idle);
}

void AiAgentComponent::to_json(json& j) const
{
	j = json{{"type", "component"}, {"class", "AiAgentComponent"}};
}

void AiAgentComponent::from_json(const json& j)
{
}

void AiAgentComponent::CreateIdle()
{
	idle = []() {};
}
void AiAgentComponent::CreateMoveTo()
{
	moveTo = []() {};
}
void AiAgentComponent::CreatePerform()
{
	perform = []() {};
}

void AiAgentComponent::Update() const
{
	if (fsm.Empty())
	{
		return;
	}

	if (fsm.Current() == nullptr)
	{
		return;
	}

	fsm.Current()();
}

void to_json(json& j, const AiAgentComponent& t)
{
	t.to_json(j);
}

void from_json(const json& j, AiAgentComponent& t)
{
	t.from_json(j);
}
