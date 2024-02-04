#include "D3E/Components/navigation/NavigationAgentComponent.h"

#include "json.hpp"

using namespace D3E;

void NavigationAgentComponent::to_json(json& j) const
{
	j = json{
		{"type", "component"},
		{"class", "NavigationAgentComponent"},
	};
}

void NavigationAgentComponent::from_json(const json& j)
{
}

void D3E::to_json(json& j, const NavigationAgentComponent& c)
{
	c.to_json(j);
}

void D3E::from_json(const json& j, NavigationAgentComponent& c)
{
	c.from_json(j);
}