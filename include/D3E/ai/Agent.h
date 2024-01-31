#pragma once

#include "Action.h"
#include "D3E/CommonHeader.h"
#include "EASTL/vector.h"
#include "Goal.h"
#include "State.h"

namespace D3E
{
	class Agent
	{
	public:
		explicit Agent(const String& name)
			: name_(name), actions_(), goal_(), currentState_()
		{
		}
		Agent(const Agent& other)
		{
			name_ = other.name_;
			goal_ = other.goal_;
			currentState_ = other.currentState_;
		}

		const Goal& GetGoal() const { return goal_; }

		const State& GetCurrentState() const { return currentState_; }

		void Init()
		{ // currentState.facts = StateManager::GetRegisteredFacts();
		}

		eastl::vector<Action> GetActions() const { return actions_; }

		void AddAction(const Action& a) { actions_.push_back(a); }

		void SetGoal(const Goal& g) { goal_ = g; }

	private:
		String name_;
		eastl::vector<Action> actions_;
		Goal goal_;
		State currentState_;
	};
} // namespace D3E