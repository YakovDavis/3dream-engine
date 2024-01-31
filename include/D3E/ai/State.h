#pragma once

#include "D3E/CommonHeader.h"
#include "EASTL/unordered_map.h"

namespace D3E
{

	struct State
	{
		String name;
		eastl::unordered_map<String, bool> facts;

		bool MatchState(const State& other) const
		{
			for (const auto& f : facts)
			{
				if (other.facts.find(f.first) == other.facts.end())
				{
					return false;
				}

				if (f.second != other.facts.at(f.first))
				{
					return false;
				}
			}

			return true;
		}

		static State CombineStates(const State& first, const State& second)
		{
			State s(first);

			for (auto& f : second.facts)
			{
				s.facts[f.first] = f.second;
			}

			return s;
		}

		State CombineState(const State& other)
		{
			State s(*this);

			for (auto& f : other.facts)
			{
				s.facts[f.first] = f.second;
			}

			return s;
		}
	};

	bool operator==(const State& lhs, const State& rhs)
	{
		return lhs.name == rhs.name && lhs.facts == rhs.facts;
	}

	bool operator!=(const State& lhs, const State& rhs)
	{
		return !(lhs == rhs);
	}
} // namespace D3E