#pragma once

#include "D3E/CommonHeader.h"
#include "State.h"

#include <string>

namespace D3E
{
	struct Goal
	{
		int priority;
		String name;
		State precondition;
		State state;

		Goal() : priority(-1), name(kEmptyGoalName), precondition(), state() {}
		Goal(const std::string& name, int priority)
			: name(name.c_str()), priority(priority), precondition(), state()
		{
		}
		Goal(const Goal& other)
			: priority(other.priority), name(other.name),
			  precondition(other.precondition), state(other.state)
		{
		}

		void AddPrecondition(const std::string& fact, bool value)
		{
			precondition.facts[fact.c_str()] = value;
		}

		void AddTarget(const std::string& fact, bool value)
		{
			state.facts[fact.c_str()] = value;
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