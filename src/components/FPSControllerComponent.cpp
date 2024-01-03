#include "D3E/Components/FPSControllerComponent.h"
#include "json.hpp"

namespace D3E
{
	void to_json(json& j, const FPSControllerComponent& t)
	{
		j = json{{"type", "component"},
		         {"class", "FPSControllerComponent"},
		         {"yaw", t.yaw},
		         {"pitch", t.pitch},
			     {"speed", t.speed},
			     {"sensitivity_x", t.sensitivityX},
				 {"sensitivity_y", t.sensitivityY},
		         {"is_LMB_activated", t.isLMBActivated}
		};
	}

	void from_json(const json& j, FPSControllerComponent& t)
	{
		j.at("yaw").get_to(t.yaw);
		j.at("pitch").get_to(t.pitch);
		j.at("speed").get_to(t.speed);
		j.at("sensitivity_x").get_to(t.sensitivityX);
		j.at("sensitivity_y").get_to(t.sensitivityY);
		j.at("is_LMB_activated").get_to(t.isLMBActivated);
	}
}
