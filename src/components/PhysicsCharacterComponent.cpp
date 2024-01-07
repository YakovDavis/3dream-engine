#include "D3E/Components/PhysicsCharacterComponent.h"
#include "core/magic_enum.hpp"
#include "json.hpp"

namespace D3E
{
	void to_json(json& j, const PhysicsCharacterComponent& t)
	{
		t.to_json(j);
	}

	void from_json(const json& j, PhysicsCharacterComponent& t)
	{
		t.from_json(j);
	}

	void PhysicsCharacterComponent::to_json(json& j) const
	{
		j = json{{"type", "component"},
		         {"class", "PhysicsCharacterComponent"},
		         {"max_slope_angle", maxSlopeAngle_},
		         {"collision_tolerance", collisionTolerance_},
		         {"time", time_},
		         {"control_movement_during_jump", controlMovementDuringJump_},
		         {"yaw", yaw_},
		         {"pitch", pitch_},
		         {"speed", speed_},
		         {"jump_speed", jumpSpeed_},
		         {"sensitivity_x", sensitivityX_},
		         {"sensitivity_y", sensitivityY_},
		         {"is_LMB_activated", isLMBActivated_},
		         {"collider_type", magic_enum::enum_name(colliderType_)},
		         {"collider_params", std::vector({colliderParams_.x, colliderParams_.y, colliderParams_.z, colliderParams_.w})},
		         {"mass", mass_},
		         {"friction", friction_},
		         {"restitution", restitution_},
		         {"has_offset_center_of_mass", hasOffsetCenterOfMass_},
		         {"center_of_mass_offset", std::vector({centerOfMassOffset_.x, centerOfMassOffset_.y, centerOfMassOffset_.z})},
		         {"velocity", std::vector({velocity_.x, velocity_.y, velocity_.z})},
		         {"angular_velocity", std::vector({angularVelocity_.x, angularVelocity_.y, angularVelocity_.z})}
		};
	}

	void PhysicsCharacterComponent::from_json(const json& j)
	{
		std::string colliderType;
		std::vector<float> colliderParams(4);
		std::vector<float> centerOfMassOffset(3);
		std::vector<float> velocity(3);
		std::vector<float> angularVelocity(3);

		j.at("max_slope_angle").get_to(maxSlopeAngle_);
		j.at("collision_tolerance").get_to(collisionTolerance_);
		j.at("time").get_to(time_);
		j.at("control_movement_during_jump").get_to(controlMovementDuringJump_);
		j.at("yaw").get_to(yaw_);
		j.at("pitch").get_to(pitch_);
		j.at("speed").get_to(speed_);
		j.at("jump_speed").get_to(jumpSpeed_);
		j.at("sensitivity_x").get_to(sensitivityX_);
		j.at("sensitivity_y").get_to(sensitivityY_);
		j.at("is_LMB_activated").get_to(isLMBActivated_);
		j.at("collider_type").get_to(colliderType);
		j.at("collider_params").get_to(colliderParams);
		j.at("mass").get_to(mass_);
		j.at("friction").get_to(friction_);
		j.at("restitution").get_to(restitution_);
		j.at("has_offset_center_of_mass").get_to(hasOffsetCenterOfMass_);
		j.at("center_of_mass_offset").get_to(centerOfMassOffset);
		j.at("velocity").get_to(velocity);
		j.at("angular_velocity").get_to(angularVelocity);

		auto c = magic_enum::enum_cast<ColliderType>(colliderType);
		if (c.has_value())
		{
			colliderType_ = c.value();
		}
		else
		{
			colliderType_ = ColliderType::BoxCollider;
		}

		colliderParams_ = DirectX::SimpleMath::Vector4(colliderParams[0], colliderParams[1], colliderParams[2], colliderParams[3]);
		centerOfMassOffset_ = DirectX::SimpleMath::Vector3(centerOfMassOffset[0], centerOfMassOffset[1], centerOfMassOffset[2]);
		velocity_ = DirectX::SimpleMath::Vector3(velocity[0], velocity[1], velocity[2]);
		angularVelocity_ = DirectX::SimpleMath::Vector3(angularVelocity[0], angularVelocity[1], angularVelocity[2]);
	}
}

