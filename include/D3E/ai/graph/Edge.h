#pragma once

#include "D3E/ai/Action.h"

namespace D3E
{
	struct Edge
	{
		static const int kDefaultActionId = -1;

		int id;
		Action action;

		Edge() : id(kDefaultActionId), action("EmptyAction") {}
		Edge(const Edge& other) = default;
		Edge(int i, const Action& action) : id(i), action(action) {}

		Edge& operator=(const Edge& other)
		{
			if (this == &other)
			{
				return *this;
			}

			id = other.id;
			action = other.action;

			return *this;
		}
	};

	struct EdgeHash
	{
		size_t operator()(const Edge& e) const
		{
			return eastl::hash<int>()(e.id);
		}
	};
} // namespace D3E

namespace eastl
{
	template<> struct hash<D3E::Edge>
	{
		std::size_t operator()(const D3E::Edge& a) const noexcept
		{
			return eastl::hash<int>()(a.id);
		}
	};
} // namespace eastl