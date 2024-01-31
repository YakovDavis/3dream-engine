#include "D3E/Components/AiAgentComponent.h"

#include "D3E/CommonHeader.h"
#include "D3E/Debug.h"
#include "D3E/ai/GoapPlanner.h"
#include "json.hpp"

using namespace D3E;

AiAgentComponent::AiAgentComponent() : agent("DefaultName"), fsm()
{
	CreateIdle();
	CreateMoveTo();
	CreatePerform();

	fsm.Push(idle);
}

void AiAgentComponent::CreateIdle()
{
	idle = [this]()
	{
		auto plan = GoapPlanner::Plan(agent);

		if (plan.empty())
		{
			Debug::LogWarning(std::format("[AiAgentComponent] : name: {}, Plan "
			                              "for goal: {} was not found!",
			                              agent.GetName().c_str(),
			                              agent.GetGoalToPlan().name.c_str())
			                      .c_str());

			fsm.Pop();
			fsm.Push(idle);

			return;
		}

		agent.SetPlan(plan);

		fsm.Pop();
		fsm.Push(perform);
	};
}
void AiAgentComponent::CreateMoveTo()
{
	moveTo = [this]() {};
}
void AiAgentComponent::CreatePerform()
{
	perform = [this]() {};
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

void AiAgentComponent::to_json(json& j) const
{
	j = json{{"type", "component"}, {"class", "AiAgentComponent"}};
}

void AiAgentComponent::from_json(const json& j)
{
}

void to_json(json& j, const AiAgentComponent& t)
{
	t.to_json(j);
}

void from_json(const json& j, AiAgentComponent& t)
{
	t.from_json(j);
}
