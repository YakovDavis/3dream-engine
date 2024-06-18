#include "D3E/Components/AiAgentComponent.h"

#include "D3E/CommonHeader.h"
#include "D3E/Debug.h"
#include "D3E/ai/FunctionalAction.h"
#include "D3E/ai/GoapPlanner.h"
#include "json.hpp"

using namespace D3E;

AiAgentComponent::AiAgentComponent()
	: agent("DefaultName"), fsm(), actionMapping(), currentGoal()
{
	CreateIdle();
	CreateMoveTo();
	CreatePerform();

	fsm.Push(idle);
}

AiAgentComponent::AiAgentComponent(const AiAgentComponent& other)
	: agent(other.agent), actionMapping(other.actionMapping), fsm(),
	  currentGoal(other.currentGoal)
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
		auto g = agent.GetGoalToPlan();

		if (g.name == kEmptyGoalName)
		{
			return;
		}

		currentGoal = g;

		auto plan = GoapPlanner::Plan(agent);

		if (plan.empty())
		{
			return;
		}

		agent.SetPlan(plan);

		fsm.Push(perform);
	};
}
void AiAgentComponent::CreateMoveTo()
{
	moveTo = [this]()
	{
		

		auto& action = actionMapping.at(agent.PeekAction());

		/*if (currentGoal.name != agent.GetGoalToPlan().name)
		{
			action.Reset();
			agent.ClearPlan();
			fsm.Pop();
		}*/

		if (action.Move())
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
				std::format("[AiAgentComponent]: Agent: {} finished its plan.",
			                agent.GetName().c_str())
					.c_str());

			fsm.Pop();

			return;
		}

		auto& action = actionMapping.at(agent.PeekAction());
		if (action.IsDone())
		{
			action.Reset();
			agent.ApplyActionEffects();
			agent.PopAction();

			return;
		}

		auto inRange = action.RequiresToBeInRange() ? action.InRange() : true;

		if (!inRange)
		{
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