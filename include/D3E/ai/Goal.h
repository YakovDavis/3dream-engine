#pragma once

#include "D3E/CommonHeader.h"
#include "State.h"

namespace D3E
{
	struct Goal
	{
		const String kNullGoalName = "Empty Goal";

		int priority;
		String name;
		State precondition;
		State state;

		Goal() : priority(-1), name(kNullGoalName), precondition(), state() {}
		Goal(const Goal& other)
			: priority(other.priority), name(other.name),
			  precondition(other.precondition), state(other.state)
		{
		}

		Goal& operator=(const Goal& other)
		{
			if (this == &other)
				return *this;

			priority = other.priority;
			name = other.name;
			precondition = other.precondition;
			state = other.state;

			return *this;
		}
	};
} // namespace D3E