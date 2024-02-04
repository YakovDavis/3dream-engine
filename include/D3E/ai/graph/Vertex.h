#pragma once

#include "D3E/CommonHeader.h"
#include "D3E/ai/State.h"

namespace D3E
{
	struct GraphVertex
	{
		int previousActionId;
		State state;

		GraphVertex() : previousActionId(-1), state() {}
		GraphVertex(int fromId, const State& s) : previousActionId(fromId), state(s)
		{
		}
		GraphVertex(const GraphVertex& other)
			: previousActionId(other.previousActionId), state(other.state)
		{
		}

		GraphVertex& operator=(const GraphVertex& other)
		{
			if (this == &other)
			{
				return *this;
			}

			previousActionId = other.previousActionId;
			state = other.state;

			return *this;
		}
	};

	bool operator==(const GraphVertex& lhs, const GraphVertex& rhs)
	{
		return lhs.previousActionId == rhs.previousActionId &&
		       lhs.state == rhs.state;
	}

	bool operator!=(const GraphVertex& lhs, const GraphVertex& rhs)
	{
		return !(lhs == rhs);
	}

	struct VertexHash
	{
		size_t operator()(const GraphVertex& v) const
		{
			return eastl::hash<int>()(v.previousActionId) +
			       eastl::hash<eastl::string>()(v.state.name);
		}
	};
} // namespace D3E

namespace std
{
	template<> struct hash<D3E::GraphVertex>
	{
		std::size_t operator()(const D3E::GraphVertex& v) const noexcept
		{
			return eastl::hash<int>()(v.previousActionId) +
			       eastl::hash<eastl::string>()(v.state.name);
		}
	};
} // namespace std