#include "D3E/Components/TransformComponent.h"
#include "json.hpp"

namespace D3E
{
	void to_json(json& j, const TransformComponent& t)
	{
		j = json{{"type", "component"},
		         {"class", "TransformComponent"},
		         {"position", std::vector({t.position.x, t.position.y, t.position.z})},
		         {"rotation", std::vector({t.rotation.x, t.rotation.y, t.rotation.z, t.rotation.w})},
				 {"scale", std::vector({t.scale.x, t.scale.y, t.scale.z})},
				 {"rel_position", std::vector({t.relativePosition.x, t.relativePosition.y, t.relativePosition.z})},
				 {"rel_rotation", std::vector({t.relativeRotation.x, t.relativeRotation.y, t.relativeRotation.z, t.relativeRotation.w})},
				 {"rel_scale", std::vector({t.relativeScale.x, t.relativeScale.y, t.relativeScale.z})},
		};
	}

	void from_json(const json& j, TransformComponent& t)
	{
		std::vector<float> position(3);
		std::vector<float> rotation(4);
		std::vector<float> scale(3);

		j.at("position").get_to(position);
		j.at("rotation").get_to(rotation);
		j.at("scale").get_to(scale);

		t.position = Vector3(position[0], position[1], position[2]);
		t.rotation = Quaternion(rotation[0], rotation[1], rotation[2], rotation[3]);
		t.scale = Vector3(scale[0], scale[1], scale[2]);

		j.at("rel_position").get_to(position);
		j.at("rel_rotation").get_to(rotation);
		j.at("rel_scale").get_to(scale);

		t.relativePosition = Vector3(position[0], position[1], position[2]);
		t.relativeRotation = Quaternion(rotation[0], rotation[1], rotation[2], rotation[3]);
		t.relativeScale = Vector3(scale[0], scale[1], scale[2]);
	}
}

