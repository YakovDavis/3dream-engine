#include "D3E/Components/FPSControllerComponent.h"
#include "json.hpp"

namespace D3E
{
	void to_json(json& j, const FPSControllerComponent& t)
	{
		t.to_json(j);
	}

	void from_json(const json& j, FPSControllerComponent& t)
	{
		t.from_json(j);
	}

	void FPSControllerComponent::to_json(json& j) const
	{
		j = json{{"type", "component"},
		         {"class", "FPSControllerComponent"},
		         {"yaw", yaw},
		         {"pitch", pitch},
		         {"speed", speed},
		         {"sensitivity_x", sensitivityX},
		         {"sensitivity_y", sensitivityY},
		         {"is_LMB_activated", isLMBActivated}
		};
	}

	void FPSControllerComponent::from_json(const json& j)
	{
		j.at("yaw").get_to(yaw);
		j.at("pitch").get_to(pitch);
		j.at("speed").get_to(speed);
		j.at("sensitivity_x").get_to(sensitivityX);
		j.at("sensitivity_y").get_to(sensitivityY);
		j.at("is_LMB_activated").get_to(isLMBActivated);
	}
}
