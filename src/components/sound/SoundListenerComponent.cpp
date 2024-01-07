#include "D3E/Components/sound/SoundListenerComponent.h"
#include "json.hpp"

namespace D3E
{
	void to_json(json& j, const SoundListenerComponent& t)
	{
		t.to_json(j);
	}

	void from_json(const json& j, SoundListenerComponent& t)
	{
		t.from_json(j);
	}

	void SoundListenerComponent::to_json(json& j) const
	{
		j = json{{"type", "component"},
		         {"class", "SoundListenerComponent"}
		};
	}

	void SoundListenerComponent::from_json(const json& j)
	{
	}
}
