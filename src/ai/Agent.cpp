#include "D3E/ai/Agent.h"

using namespace D3E;

Agent::Agent(const String& name)
	: name_(name), actions_(), currentState_(), plan_(), goals_()
{
}

Agent::Agent(const Agent& other)
{
	name_ = other.name_;
	goals_ = other.goals_;
	currentState_ = other.currentState_;
	plan_ = other.plan_;
}

void Agent::SetName(const String& name)
{
	name_ = name;
}

const String& Agent::GetName() const
{
	return name_;
}

void Agent::AddGoal(const Goal& g)
{
	goals_.push_back(g);
}

void Agent::RemoveGoal(const String& name)
{
	for (auto it = goals_.begin(); it != goals_.end();)
	{
		if (it->name == name)
		{
			goals_.erase(it);

			return;
		}

		++it;
	}
}

const eastl::vector<Goal>& Agent::GetGoals() const
{
	return goals_;
}

Goal Agent::GetGoalToPlan() const
{
	Goal goal;

	if (goals_.empty())
	{
		return goal;
	}

	for (const auto& g : goals_)
	{
		if (g.state.MatchState(currentState_) && g.priority > goal.priority)
		{
			goal = g;
		}
	}

	return goal;
}

const State& Agent::GetCurrentState() const
{
	return currentState_;
}

const eastl::vector<Action>& Agent::GetActions() const
{
	return actions_;
}

void Agent::AddAction(const Action& a)
{
	actions_.push_back(a);
}

void Agent::SetPlan(const eastl::vector<Action>& actions)
{
	plan_ = {};

	for (const auto& a : actions)
	{
		plan_.push(a);
	}
}

void Agent::ClearPlan()
{
	plan_ = {};
}

const Action& Agent::PeekAction() const
{
	return plan_.front();
}

Action Agent::PopAction()
{
	Action a = plan_.front();
	plan_.pop();

	return a;
}

bool Agent::HasPlan() const
{
	return !plan_.empty();
}