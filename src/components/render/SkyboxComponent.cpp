#include "D3E/Components/render/SkyboxComponent.h"
#include "json.hpp"

namespace D3E
{
	void to_json(json& j, const SkyboxComponent& t)
	{
		t.to_json(j);
	}

	void from_json(const json& j, SkyboxComponent& t)
	{
		t.from_json(j);
	}

	void SkyboxComponent::to_json(json& j) const
	{
		j = json{{"type", "component"},
		         {"class", "SkyboxComponent"}
		};
	}

	void SkyboxComponent::from_json(const json& j)
	{
	}
}
