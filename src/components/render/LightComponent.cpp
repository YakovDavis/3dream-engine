#include "D3E/Components/render/LightComponent.h"
#include "core/magic_enum.hpp"
#include "json.hpp"

namespace D3E
{
	void to_json(json& j, const LightComponent& t)
	{
		j = json{{"type", "component"},
		         {"class", "StaticMeshComponent"},
		         {"light_type", magic_enum::enum_name(t.lightType)},
		         {"offset", std::vector({t.offset.x, t.offset.y, t.offset.z})},
		         {"direction", std::vector({t.direction.x, t.direction.y, t.direction.z})},
		         {"color", std::vector({t.color.x, t.color.y, t.color.z})},
		         {"intensity", t.intensity},
		         {"casts_shadows", t.castsShadows}
		};
	}

	void from_json(const json& j, LightComponent& t)
	{
		std::string lightType;
		std::vector<float> offset(3);
		std::vector<float> direction(3);
		std::vector<float> color(3);

		j.at("light_type").get_to(lightType);
		j.at("offset").get_to(offset);
		j.at("direction").get_to(direction);
		j.at("color").get_to(color);
		j.at("intensity").get_to(t.intensity);
		j.at("casts_shadows").get_to(t.castsShadows);

		auto c = magic_enum::enum_cast<LightType>(lightType);
		if (c.has_value())
		{
			t.lightType = c.value();
		}
		else
		{
			t.lightType = LightType::Directional;
		}

		t.offset = Vector3(offset[0], offset[1], offset[2]);
		t.direction = Vector3(direction[0], direction[1], direction[2]);
		t.color = Vector3(color[0], color[1], color[2]);
	}
}
