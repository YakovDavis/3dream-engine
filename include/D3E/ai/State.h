#pragma once

#include "D3E/CommonHeader.h"
#include "EASTL/unordered_map.h"

namespace D3E
{

	struct State
	{
		String name;
		eastl::unordered_map<String, bool> facts;

		bool MatchState(const State& other) const;
		static State CombineStates(const State& first, const State& second);
		State CombineState(const State& other);
	};

	bool operator==(const State& lhs, const State& rhs);

	bool operator!=(const State& lhs, const State& rhs);

} // namespace D3E