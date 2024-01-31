#pragma once

#include "Action.h"
#include "D3E/CommonHeader.h"
#include "EASTL/queue.h"
#include "EASTL/vector.h"
#include "FSM.h"
#include "Goal.h"
#include "State.h"

namespace D3E
{
	class Agent
	{
	public:
		explicit Agent(const String& name);
		Agent(const Agent& other);

		void SetName(const String& name);
		const String& GetName() const;
		void AddGoal(const Goal& g);
		void RemoveGoal(const String& name);
		const eastl::vector<Goal>& GetGoals() const;
		Goal GetGoalToPlan() const;
		const State& GetCurrentState() const;
		const eastl::vector<Action>& GetActions() const;
		void AddAction(const Action& a);
		void SetPlan(const eastl::vector<Action>& actions);

	private:
		eastl::queue<Action> plan_;
		eastl::vector<Goal> goals_;
		String name_;
		eastl::vector<Action> actions_;
		State currentState_;
	};
} // namespace D3E