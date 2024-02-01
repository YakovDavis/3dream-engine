#pragma once

#include "D3E/ai/Action.h"
#include "D3E/ai/Agent.h"
#include "D3E/ai/Goal.h"
#include "D3E/ai/State.h"
#include "D3E/ai/graph/Edge.h"
#include "D3E/ai/graph/Vertex.h"
#include "D3E/structures/PriorityQueue.h"
#include "EASTL/map.h"
#include "EASTL/vector.h"

#include <format>

namespace D3E
{
	class GoapPlanner
	{
	public:
		GoapPlanner() = delete;
		GoapPlanner(const GoapPlanner& other) = delete;

		static eastl::vector<GraphVertex>
		GetNeighbors(const GraphVertex& current, const eastl::vector<Edge>& edges)
		{
			eastl::vector<GraphVertex> neighbors;

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
			eastl::vector<Edge> edges;

			for (auto& action : a.GetActions())
			{
				edges.push_back({(int)edges.size(), action});
			}

			GraphVertex startState = GraphVertex(-1, a.GetCurrentState());
			GraphVertex currentState = startState;

			Goal goal = a.GetGoalToPlan();
			if (goal.name == kNullGoalName)
			{
				return {};
			}

			State goalState = goal.state;

			PriorityQueue<GraphVertex, int> frontier{};
			eastl::unordered_map<GraphVertex, GraphVertex, VertexHash> previousState;
			eastl::unordered_map<GraphVertex, int, VertexHash> cumulativeCost;
			cumulativeCost[currentState] = 0;

			frontier.Push(currentState, 0);

			while (!frontier.IsEmpty())
			{
				currentState = frontier.Pop();

				if (goalState.MatchState(currentState.state))
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
							newCost + Heuristic(currentState.state, goalState);

						frontier.Push(nextState, newCost);
						previousState[nextState] = currentState;
					}
				}
			}

			eastl::vector<Action> path;

			while (currentState != startState)
			{
				path.push_back(edges[currentState.previousActionId].action);
				currentState = previousState[currentState];
			}

			eastl::reverse(path.begin(), path.end());

			return path;
		}
	};
} // namespace D3E