#pragma once

#include "D3E/CommonHeader.h"
#include "State.h"

#include <string>

namespace D3E
{
	class Action
	{
	public:
		Action() = default;
		Action(const Action& other) = default;
		explicit Action(const std::string& name) : name_(name.c_str()), cost_(1)
		{
		}

		const String& GetName() const { return name_; }

		int GetCost() const { return cost_; }

		void SetCost(int c) { cost_ = c; }

		const State& GetPreconditions() const { return preconditions_; }

		const State& GetEffects() const { return effects_; }

		void AddPrecondition(const std::string& fact, bool value)
		{
			preconditions_.facts[fact.c_str()] = value;
		}

		void AddEffect(const std::string& fact, bool value)
		{
			effects_.facts[fact.c_str()] = value;
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