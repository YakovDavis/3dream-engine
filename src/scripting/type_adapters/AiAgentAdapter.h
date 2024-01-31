#pragma once

#include "D3E/Components/AiAgentComponent.h"
#include "D3E/ai/Action.h"
#include "D3E/ai/FunctionalAction.h"
#include "D3E/ai/Goal.h"
#include "D3E/ai/State.h"
#include "sol/sol.hpp"

#include <string>

namespace D3E
{
	class AiAgentAdapter
	{
	public:
		AiAgentAdapter(AiAgentComponent* ac) : agent_(ac) {}

		void AddGoal(const Goal& g) { agent_->agent.AddGoal(g); }
		void AddAction(const Action& a, const sol::function& f)
		{
			agent_->agent.AddAction(a);
			agent_->actionMapping[a] =
				FunctionalAction([&f]() { return static_cast<bool>(f()); });
		}
		void SetName(const std::string& name)
		{
			agent_->agent.SetName(name.c_str());
		}

	private:
		AiAgentComponent* agent_;
	};
} // namespace D3E