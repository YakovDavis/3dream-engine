#include "D3E/Components/MouseComponent.h"
#include "json.hpp"

namespace D3E
{
	void to_json(json& j, const MouseComponent& t)
	{
		t.to_json(j);
	}

	void from_json(const json& j, MouseComponent& t)
	{
		t.from_json(j);
	}

	void MouseComponent::to_json(json& j) const
	{
		j = json{{"type", "component"},
		         {"class", "MouseComponent"},
		         {"position", std::vector({position.x, position.y})},
		         {"delta", std::vector({delta.x, delta.y})}
		};
	}

	void MouseComponent::from_json(const json& j)
	{
		std::vector<float> tmp_position(2);
		std::vector<float> tmp_delta(2);

		j.at("position").get_to(tmp_position);
		j.at("delta").get_to(tmp_delta);

		position = Vector2(tmp_position[0], tmp_position[1]);
		delta = Vector2(tmp_delta[0], tmp_delta[1]);
	}
}
