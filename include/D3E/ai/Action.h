#pragma once

#include "D3E/CommonHeader.h"
#include "State.h"

namespace D3E
{
	class Action
	{
	public:
		Action() = default;
		Action(const Action& other) = default;
		explicit Action(const String& name) : name_(name), cost_(1) {}

		const String& GetName() const { return name_; }

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

	bool operator==(const Action& lhs, const Action& rhs);
	bool operator!=(const Action& lhs, const Action& rhs);

	struct ActionHash
	{
		size_t operator()(const Action& e) const
		{
			return eastl::hash<eastl::string>()(e.GetName());
		}
	};
} // namespace D3E