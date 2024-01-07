#include "D3E/Components/TransformComponent.h"
#include "json.hpp"

namespace D3E
{
	void to_json(json& j, const TransformComponent& t)
	{
		t.to_json(j);
	}

	void from_json(const json& j, TransformComponent& t)
	{
		t.from_json(j);
	}

	void TransformComponent::to_json(json& j) const
	{
		j = json{{"type", "component"},
			{"class", "TransformComponent"},
			{"position", std::vector({position.x, position.y, position.z})},
			{"rotation", std::vector({rotation.x, rotation.y, rotation.z, rotation.w})},
			{"scale", std::vector({scale.x, scale.y, scale.z})},
			{"rel_position", std::vector({relativePosition.x, relativePosition.y, relativePosition.z})},
			{"rel_rotation", std::vector({relativeRotation.x, relativeRotation.y, relativeRotation.z, relativeRotation.w})},
			{"rel_scale", std::vector({relativeScale.x, relativeScale.y, relativeScale.z})},
		};
	}

	void TransformComponent::from_json(const json& j)
	{
		std::vector<float> tmp_position(3);
		std::vector<float> tmp_rotation(4);
		std::vector<float> tmp_scale(3);

		j.at("position").get_to(tmp_position);
		j.at("rotation").get_to(tmp_rotation);
		j.at("scale").get_to(tmp_scale);

		position = Vector3(tmp_position[0], tmp_position[1], tmp_position[2]);
		rotation = Quaternion(tmp_rotation[0], tmp_rotation[1], tmp_rotation[2], tmp_rotation[3]);
		scale = Vector3(tmp_scale[0], tmp_scale[1], tmp_scale[2]);

		j.at("rel_position").get_to(tmp_position);
		j.at("rel_rotation").get_to(tmp_rotation);
		j.at("rel_scale").get_to(tmp_scale);

		relativePosition = Vector3(tmp_position[0], tmp_position[1], tmp_position[2]);
		relativeRotation = Quaternion(tmp_rotation[0], tmp_rotation[1], tmp_rotation[2], tmp_rotation[3]);
		relativeScale = Vector3(tmp_scale[0], tmp_scale[1], tmp_scale[2]);
	}
}

