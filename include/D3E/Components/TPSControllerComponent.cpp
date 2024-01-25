#include "TPSControllerComponent.h"
#include "json.hpp"

using namespace DirectX::SimpleMath;

void D3E::TPSControllerComponent::to_json(json& j) const
{
	j = json{{"type", "component"},
	         {"class", "TPSControllerComponent"},
	         {"radius", radius},
	         {"phi", phi},
	         {"theta", theta},
	         {"sensitivity_x", sensitivityX},
	         {"sensitivity_y", sensitivityY},
	         {"is_RMB_activated", isRMBActivated},
			 {"limit_theta", limitTheta},
			 {"upper_theta_limit", upperThetaLimit},
			 {"lower_theta_limit", lowerThetaLimit},
			 {"invert_x_axis", invertXAxis},
			 {"invert_y_axis", invertYAxis}
	};
}

void D3E::TPSControllerComponent::from_json(const json& j)
{
	j.at("radius").get_to(radius);
	j.at("phi").get_to(phi);
	j.at("theta").get_to(theta);
	j.at("sensitivity_x").get_to(sensitivityX);
	j.at("sensitivity_y").get_to(sensitivityY);
	j.at("is_RMB_activated").get_to(isRMBActivated);
	j.at("limit_theta").get_to(limitTheta);
	j.at("upper_theta_limit").get_to(upperThetaLimit);
	j.at("lower_theta_limit").get_to(lowerThetaLimit);
	j.at("invert_x_axis").get_to(invertXAxis);
	j.at("invert_y_axis").get_to(invertYAxis);
}

void to_json(json& j, const D3E::TPSControllerComponent& t)
{
	t.to_json(j);
}

void from_json(const json& j, D3E::TPSControllerComponent& t)
{
	t.from_json(j);
}