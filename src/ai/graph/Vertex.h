#pragma once

#include "ai/State.h"

namespace D3E
{
	struct Vertex
	{
		int previousActionId;
		State state;

		Vertex() : previousActionId(-1), state() {}
		Vertex(int fromId, const State& s) : previousActionId(fromId), state(s)
		{
		}
		Vertex(const Vertex& other)
			: previousActionId(other.previousActionId), state(other.state)
		{
		}
	};

	bool operator==(const Vertex& lhs, const Vertex& rhs)
	{
		return lhs.previousActionId == rhs.previousActionId &&
		       lhs.state == rhs.state;
	}

	bool operator!=(const Vertex& lhs, const Vertex& rhs)
	{
		return !(lhs == rhs);
	}
} // namespace D3E