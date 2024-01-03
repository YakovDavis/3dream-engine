#include "D3E/Components/PhysicsCharacterComponent.h"
#include "core/magic_enum.hpp"
#include "json.hpp"

namespace D3E
{
	void to_json(json& j, const PhysicsCharacterComponent& t)
	{
		j = json{{"type", "component"},
		         {"class", "PhysicsCharacterComponent"},
		         {"max_slope_angle", t.maxSlopeAngle_},
		         {"collision_tolerance", t.collisionTolerance_},
		         {"time", t.time_},
		         {"control_movement_during_jump", t.controlMovementDuringJump_},
		         {"yaw", t.yaw_},
		         {"pitch", t.pitch_},
		         {"speed", t.speed_},
		         {"jump_speed", t.jumpSpeed_},
		         {"sensitivity_x", t.sensitivityX_},
		         {"sensitivity_y", t.sensitivityY_},
		         {"is_LMB_activated", t.isLMBActivated_},
		         {"collider_type", magic_enum::enum_name(t.colliderType_)},
		         {"collider_params", std::vector({t.colliderParams_.x, t.colliderParams_.y, t.colliderParams_.z, t.colliderParams_.w})},
		         {"mass", t.mass_},
		         {"friction", t.friction_},
		         {"restitution", t.restitution_},
		         {"has_offset_center_of_mass", t.hasOffsetCenterOfMass_},
		         {"center_of_mass_offset", std::vector({t.centerOfMassOffset_.x, t.centerOfMassOffset_.y, t.centerOfMassOffset_.z})},
		         {"velocity", std::vector({t.velocity_.x, t.velocity_.y, t.velocity_.z})},
		         {"angular_velocity", std::vector({t.angularVelocity_.x, t.angularVelocity_.y, t.angularVelocity_.z})}
		};
	}

	void from_json(const json& j, PhysicsCharacterComponent& t)
	{
		std::string colliderType;
		std::vector<float> colliderParams(4);
		std::vector<float> centerOfMassOffset(3);
		std::vector<float> velocity(3);
		std::vector<float> angularVelocity(3);

		j.at("max_slope_angle").get_to(t.maxSlopeAngle_);
		j.at("collision_tolerance").get_to(t.collisionTolerance_);
		j.at("time").get_to(t.time_);
		j.at("control_movement_during_jump").get_to(t.controlMovementDuringJump_);
		j.at("yaw").get_to(t.yaw_);
		j.at("pitch").get_to(t.pitch_);
		j.at("speed").get_to(t.speed_);
		j.at("jump_speed").get_to(t.jumpSpeed_);
		j.at("sensitivity_x").get_to(t.sensitivityX_);
		j.at("sensitivity_y").get_to(t.sensitivityY_);
		j.at("is_LMB_activated").get_to(t.isLMBActivated_);
		j.at("collider_type").get_to(colliderType);
		j.at("collider_params").get_to(colliderParams);
		j.at("mass").get_to(t.mass_);
		j.at("friction").get_to(t.friction_);
		j.at("restitution").get_to(t.restitution_);
		j.at("has_offset_center_of_mass").get_to(t.hasOffsetCenterOfMass_);
		j.at("center_of_mass_offset").get_to(centerOfMassOffset);
		j.at("velocity").get_to(velocity);
		j.at("angular_velocity").get_to(angularVelocity);

		auto c = magic_enum::enum_cast<ColliderType>(colliderType);
		if (c.has_value())
		{
			t.colliderType_ = c.value();
		}
		else
		{
			t.colliderType_ = ColliderType::BoxCollider;
		}

		t.colliderParams_ = DirectX::SimpleMath::Vector4(colliderParams[0], colliderParams[1], colliderParams[2], colliderParams[3]);
		t.centerOfMassOffset_ = DirectX::SimpleMath::Vector3(centerOfMassOffset[0], centerOfMassOffset[1], centerOfMassOffset[2]);
		t.velocity_ = DirectX::SimpleMath::Vector3(velocity[0], velocity[1], velocity[2]);
		t.angularVelocity_ = DirectX::SimpleMath::Vector3(angularVelocity[0], angularVelocity[1], angularVelocity[2]);
	}
}

