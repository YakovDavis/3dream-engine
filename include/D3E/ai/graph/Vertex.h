#pragma once

#include "D3E/CommonHeader.h"
#include "D3E/ai/State.h"

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

	struct VertexHash
	{
		size_t operator()(const Vertex& v) const
		{
			return eastl::hash<int>()(v.previousActionId) +
			       eastl::hash<eastl::string>()(v.state.name);
		}
	};
} // namespace D3E

namespace std
{
	template<> struct hash<D3E::Vertex>
	{
		std::size_t operator()(const D3E::Vertex& v) const noexcept
		{
			return eastl::hash<int>()(v.previousActionId) +
			       eastl::hash<eastl::string>()(v.state.name);
		}
	};
} // namespace std