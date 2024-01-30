#include "D3E/Components/AiComponent.h"

#include "D3E/CommonHeader.h"
#include "json.hpp"

void D3E::AiComponent::to_json(json& j) const
{
	j = json{{"type", "component"}, {"class", "AiComponent"}};
}

void D3E::AiComponent::from_json(const json& j)
{
}

void to_json(json& j, const D3E::AiComponent& t)
{
	t.to_json(j);
}

void from_json(const json& j, D3E::AiComponent& t)
{
	t.from_json(j);
}
