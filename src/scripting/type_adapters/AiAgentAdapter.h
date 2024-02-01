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

		void AddAction(const Action& a, sol::function action)
		{
			if (action == sol::nil)
			{
				Debug::LogError(
					"[AiAgentAdapter] : AddAction(): function is null.");
				return;
			}

			agentComponent->agent.AddAction(a);
			agentComponent->actionMapping[a] = FunctionalAction(
				[action]()
				{
					bool result = action();

					return result;
				});
		}

		void AddAction(const Action& a, sol::table self, sol::function action)
		{
			if (self == sol::nil)
			{
				Debug::LogError(
					"[AiAgentAdapter] : AddAction(): self is null.");

				return;
			}

			if (action == sol::nil)
			{
				Debug::LogError(
					"[AiAgentAdapter] : AddAction(): action is null.");

				return;
			}

			agentComponent->agent.AddAction(a);
			agentComponent->actionMapping[a] = FunctionalAction(
				[action, self]()
				{
					bool result = action(self);

					return result;
				});
		}

		void AddAction(const Action& a, sol::table self, sol::function action,
		               sol::function inRange)
		{
			if (self == sol::nil)
			{
				Debug::LogError(
					"[AiAgentAdapter] : AddAction(): self is null.");

				return;
			}

			if (action == sol::nil)
			{
				Debug::LogError(
					"[AiAgentAdapter] : AddAction(): action is null.");

				return;
			}

			if (inRange == sol::nil)
			{
				Debug::LogError(
					"[AiAgentAdapter] : AddAction(): inRange is null.");

				return;
			}

			agentComponent->agent.AddAction(a);
			agentComponent->actionMapping[a] = FunctionalAction(
				[action, self]()
				{
					bool result = action(self);

					return result;
				},
				[inRange, self]()
				{
					bool result = inRange(self);

					return result;
				});
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