#include "D3E/ai/State.h"

using namespace D3E;

bool State::MatchState(const State& other) const
{
	if (facts.empty())
	{
		return true;
	}

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

State State::CombineStates(const State& first, const State& second)
{
	State s(first);

	for (auto& f : second.facts)
	{
		s.facts[f.first] = f.second;
	}

	return s;
}

State State::CombineState(const State& other)
{
	State s(*this);

	for (auto& f : other.facts)
	{
		s.facts[f.first] = f.second;
	}

	return s;
}

bool D3E::operator==(const State& lhs, const State& rhs)
{
	return lhs.name == rhs.name && lhs.facts == rhs.facts;
}

bool D3E::operator!=(const State& lhs, const State& rhs)
{
	return !(lhs == rhs);
}