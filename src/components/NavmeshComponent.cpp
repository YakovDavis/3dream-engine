#include "D3E/Components/navigation/NavmeshComponent.h"

#include "json.hpp"

using namespace D3E;

void NavmeshComponent::to_json(json& j) const
{
	j = json{
		{"type", "component"},
		{"class", "NavmeshComponent"},
	};
}

void NavmeshComponent::from_json(const json& j)
{
}

void to_json(json& j, const NavmeshComponent& c)
{
	c.to_json(j);
}

void from_json(const json& j, NavmeshComponent& c)
{
	c.from_json(j);
}