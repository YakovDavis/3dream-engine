#pragma once

#include "D3E/CommonHeader.h"
#include "ai/State.h"

namespace D3E
{
	class Action
	{
	public:
		Action(const Action& other) = default;
		explicit Action(const String& name) : name_(name), cost_(1) {}

		int GetCost() const { return cost_; }

		void SetCost(int c) { cost_ = c; }

		const State& GetPreconditions() const { return preconditions_; }

		const State& GetEffects() const { return effects_; }

		void AddPrecondition(const String& fact, bool value)
		{
			preconditions_.facts[fact] = value;
		}

		void AddEffect(const String& fact, bool value)
		{
			effects_.facts[fact] = value;
		}

		void Print() const {}

	protected:
		String name_;
		int cost_;
		State preconditions_;
		State effects_;
	};
} // namespace D3E