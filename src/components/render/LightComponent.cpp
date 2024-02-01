#include "D3E/Components/render/LightComponent.h"
#include "core/magic_enum.hpp"
#include "json.hpp"

using namespace DirectX::SimpleMath;

namespace D3E
{
	void to_json(json& j, const LightComponent& t)
	{
		t.to_json(j);
	}

	void from_json(const json& j, LightComponent& t)
	{
		t.from_json(j);
	}

	void LightComponent::to_json(json& j) const
	{
		j = json{{"type", "component"},
		         {"class", "LightComponent"},
		         {"light_type", magic_enum::enum_name(lightType)},
		         {"offset", std::vector({offset.x, offset.y, offset.z})},
		         {"direction", std::vector({direction.x, direction.y, direction.z})},
		         {"color", std::vector({color.x, color.y, color.z})},
		         {"intensity", intensity},
		         {"casts_shadows", castsShadows},
		         {"attenuation_limit_radius", attenuationLimitRadius}
		};
	}

	void LightComponent::from_json(const json& j)
	{
		std::string tmp_lightType;
		std::vector<float> tmp_offset(3);
		std::vector<float> tmp_direction(3);
		std::vector<float> tmp_color(3);

		j.at("light_type").get_to(tmp_lightType);
		j.at("offset").get_to(tmp_offset);
		j.at("direction").get_to(tmp_direction);
		j.at("color").get_to(tmp_color);
		j.at("intensity").get_to(intensity);
		j.at("casts_shadows").get_to(castsShadows);

		if (j.contains("attenuation_limit_radius"))
		{
			j.at("attenuation_limit_radius").get_to(attenuationLimitRadius);
		}
		else
		{
			attenuationLimitRadius = 50.0f;
		}

		auto c = magic_enum::enum_cast<LightType>(tmp_lightType);
		if (c.has_value())
		{
			lightType = c.value();
		}
		else
		{
			lightType = LightType::Directional;
		}

		offset = Vector3(tmp_offset[0], tmp_offset[1], tmp_offset[2]);
		direction = Vector3(tmp_direction[0], tmp_direction[1], tmp_direction[2]);
		color = Vector3(tmp_color[0], tmp_color[1], tmp_color[2]);
	}
}
