#pragma once

#include "EASTL/map.h"
#include "EASTL/vector.h"
#include "ai/Action.h"
#include "ai/Agent.h"
#include "ai/Goal.h"
#include "ai/State.h"
#include "ai/graph/Edge.h"
#include "ai/graph/Vertex.h"
#include "ai/structures/PriorityQueue.h"

#include <format>

namespace D3E
{
	class GoapPlanner
	{
	public:
		GoapPlanner() = delete;

		static eastl::vector<Vertex>
		GetNeighbors(const Vertex& current, const std::vector<Edge>& edges)
		{
			eastl::vector<Vertex> neighbors;

			for (auto& e : edges)
			{
				if (e.action.GetPreconditions().MatchState(current.state))
				{
					auto nextState = State::CombineStates(
						current.state, e.action.GetEffects());
					nextState.name += String(
						std::format("-{}", std::to_string(e.id)).c_str());

					neighbors.push_back({e.id, nextState});
				}
			}

			return neighbors;
		}

		static int Heuristic(const State& from, const State& to)
		{
			eastl::map<String, bool> fromUnion;
			eastl::map<String, bool> toUnion;

			for (const auto& f : from.facts)
			{
				fromUnion[f.first] = f.second;
			}

			for (const auto& f : to.facts)
			{
				toUnion[f.first] = f.second;
			}

			for (const auto& f : toUnion)
			{
				if (fromUnion.find(f.first) != fromUnion.end())
				{
					continue;
				}

				fromUnion[f.first] = false;
			}

			for (const auto& f : fromUnion)
			{
				if (toUnion.find(f.first) != toUnion.end())
				{
					continue;
				}

				toUnion[f.first] = false;
			}

			auto sum = 0;

			for (const auto& f : fromUnion)
			{
				sum += std::abs(fromUnion[f.first] - toUnion[f.first]);
			}

			return sum;
		}

		static eastl::vector<Action> Plan(const Agent& a)
		{
			auto actions = a.GetActions();
			std::vector<Edge> edges;

			for (auto& a : actions)
			{
				edges.push_back({(int)edges.size(), a});
			}

			Vertex startState = Vertex(-1, a.GetCurrentState());
			Vertex currentState = startState;
			State goal = a.GetGoal().state;

			PriorityQueue<Vertex, int> frontier;
			eastl::unordered_map<Vertex, Vertex> previousState;
			eastl::unordered_map<Vertex, int> cumulativeCost;
			cumulativeCost[currentState] = 0;

			frontier.Push(currentState, 0);

			while (!frontier.IsEmpty())
			{
				currentState = frontier.Pop();

				if (goal.MatchState(currentState.state))
				{
					break;
				}

				for (auto& nextState : GetNeighbors(currentState, edges))
				{
					auto newCost =
						cumulativeCost[currentState] +
						edges[nextState.previousActionId].action.GetCost();

					if (cumulativeCost.find(nextState) ==
					        cumulativeCost.end() ||
					    newCost < cumulativeCost[nextState])
					{
						cumulativeCost[nextState] = newCost;
						auto priority =
							newCost + Heuristic(currentState.state, goal);

						frontier.Push(nextState, newCost);
						previousState[nextState] = currentState;
					}
				}
			}

			std::vector<Action> path;

			while (currentState != startState)
			{
				path.push_back(edges[currentState.previousActionId].action);
				currentState = previousState[currentState];
			}

			std::reverse(path.begin(), path.end());
		}
	};
} // namespace D3E