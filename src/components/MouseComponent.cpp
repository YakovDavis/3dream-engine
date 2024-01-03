#include "D3E/Components/MouseComponent.h"
#include "json.hpp"

namespace D3E
{
	void to_json(json& j, const MouseComponent& t)
	{
		j = json{{"type", "component"},
		         {"class", "MouseComponent"},
		         {"position", std::vector({t.position.x, t.position.y})},
		         {"delta", std::vector({t.delta.x, t.delta.y})}
		};
	}

	void from_json(const json& j, MouseComponent& t)
	{
		std::vector<float> position(2);
		std::vector<float> delta(2);

		j.at("position").get_to(position);
		j.at("delta").get_to(delta);

		t.position = Vector2(position[0], position[1]);
		t.delta = Vector2(delta[0], delta[1]);
	}
}
