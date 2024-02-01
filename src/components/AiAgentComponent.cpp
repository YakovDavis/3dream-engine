#include "D3E/Components/AiAgentComponent.h"

#include "D3E/CommonHeader.h"
#include "D3E/Debug.h"
#include "D3E/ai/FunctionalAction.h"
#include "D3E/ai/GoapPlanner.h"
#include "json.hpp"

using namespace D3E;

AiAgentComponent::AiAgentComponent()
	: agent("DefaultName"), fsm(), actionMapping()
{
	CreateIdle();
	CreateMoveTo();
	CreatePerform();

	fsm.Push(idle);
}

AiAgentComponent::AiAgentComponent(const AiAgentComponent& other)
	: agent(other.agent), actionMapping(other.actionMapping), fsm()
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
			Debug::LogWarning(
				std::format("[AiAgentComponent] : Agent: {}, Plan "
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
	moveTo = [this]()
	{
		Debug::LogMessage(
			std::format("[AiAgentComponent] : moveTo(): Agent: {} moving.",
		                agent.GetName().c_str())
				.c_str());

		auto& action = actionMapping.at(agent.PeekAction());

		if (action.InRange())
		{
			Debug::LogMessage(
				std::format(
					"[AiAgentComponent] : moveTo(): Agent: {} reached target.",
					agent.GetName().c_str())
					.c_str());

			fsm.Pop();
		}
	};
}
void AiAgentComponent::CreatePerform()
{
	perform = [this]()
	{
		if (!agent.HasPlan())
		{
			Debug::LogWarning(
				std::format("[AiAgentComponent]: Agent: {} done its plan.",
			                agent.GetName().c_str())
					.c_str());

			fsm.Pop();
			fsm.Push(idle);

			return;
		}

		auto& action = actionMapping.at(agent.PeekAction());
		if (action.IsDone())
		{
			agent.PopAction();

			return;
		}

		auto inRange = action.RequiresToBeInRange() ? action.InRange() : true;

		if (!inRange)
		{
			fsm.Pop();
			fsm.Push(moveTo);

			return;
		}

		action.Perform();
	};
}

void AiAgentComponent::Update()
{
	fsm.Update();
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
