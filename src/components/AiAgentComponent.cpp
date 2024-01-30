#include "D3E/Components/AiAgentComponent.h"

#include "D3E/CommonHeader.h"
#include "json.hpp"

void D3E::AiAgentComponent::to_json(json& j) const
{
	j = json{{"type", "component"}, {"class", "AiAgentComponent"}};
}

void D3E::AiAgentComponent::from_json(const json& j)
{
}

void to_json(json& j, const D3E::AiAgentComponent& t)
{
	t.to_json(j);
}

void from_json(const json& j, D3E::AiAgentComponent& t)
{
	t.from_json(j);
}
