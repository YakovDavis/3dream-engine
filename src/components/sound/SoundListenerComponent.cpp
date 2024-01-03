#include "D3E/Components/sound/SoundListenerComponent.h"
#include "json.hpp"

namespace D3E
{
	void to_json(json& j, const SoundListenerComponent& t)
	{
		j = json{{"type", "component"},
		         {"class", "SoundListenerComponent"}
		};
	}

	void from_json(const json& j, SoundListenerComponent& t)
	{
	}
}
