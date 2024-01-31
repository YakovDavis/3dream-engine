#pragma once

#include "D3E/Components/AiAgentComponent.h"
#include "D3E/Debug.h"
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
		AiAgentAdapter(AiAgentComponent* ac) : agentComponent(ac) {}

		void AddGoal(const Goal& g) { agentComponent->agent.AddGoal(g); }

		void AddAction(const Action& a, const sol::function& f)
		{
			if (!f)
			{
				Debug::LogError(
					"[AiAgentAdapter] : AddAction(): function is null.");
				return;
			}

			agentComponent->agent.AddAction(a);
			agentComponent->actionMapping[a] =
				FunctionalAction([&f]() { return static_cast<bool>(f()); });
		}

		void SetName(const std::string& name)
		{
			agentComponent->agent.SetName(name.c_str());
		}

		void SetStateFact(const std::string& key, bool value)
		{
			agentComponent->agent.SetStateFact(key, value);
		}

	private:
		AiAgentComponent* agentComponent;
	};
} // namespace D3E