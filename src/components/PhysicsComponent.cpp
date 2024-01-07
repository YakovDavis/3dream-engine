#include "D3E/Components/PhysicsComponent.h"
#include "core/magic_enum.hpp"
#include "json.hpp"

namespace D3E
{
	void to_json(json& j, const PhysicsComponent& t)
	{
		t.to_json(j);
	}

	void from_json(const json& j, PhysicsComponent& t)
	{
		t.from_json(j);
	}

	void PhysicsComponent::to_json(json& j) const
	{
		j = json{{"type", "component"},
		         {"class", "PhysicsComponent"},
		         {"collider_type", magic_enum::enum_name(colliderType_)},
		         {"collider_params", std::vector({colliderParams_.x, colliderParams_.y, colliderParams_.z, colliderParams_.w})},
		         {"mass", mass_},
		         {"friction", friction_},
		         {"restitution", restitution_},
		         {"has_offset_center_of_mass", hasOffsetCenterOfMass_},
		         {"center_of_mass_offset", std::vector({centerOfMassOffset_.x, centerOfMassOffset_.y, centerOfMassOffset_.z})},
		         {"motion_type", magic_enum::enum_name(motionType_)},
		         {"velocity", std::vector({velocity_.x, velocity_.y, velocity_.z})},
		         {"angular_velocity", std::vector({angularVelocity_.x, angularVelocity_.y, angularVelocity_.z})}
		};
	}

	void PhysicsComponent::from_json(const json& j)
	{
		std::string colliderType;
		std::vector<float> colliderParams(4);
		std::vector<float> centerOfMassOffset(3);
		std::string motionType;
		std::vector<float> velocity(3);
		std::vector<float> angularVelocity(3);

		j.at("collider_type").get_to(colliderType);
		j.at("collider_params").get_to(colliderParams);
		j.at("mass").get_to(mass_);
		j.at("friction").get_to(friction_);
		j.at("restitution").get_to(restitution_);
		j.at("has_offset_center_of_mass").get_to(hasOffsetCenterOfMass_);
		j.at("center_of_mass_offset").get_to(centerOfMassOffset);
		j.at("motion_type").get_to(motionType);
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

		auto m = magic_enum::enum_cast<JPH::EMotionType>(motionType);
		if (m.has_value())
		{
			motionType_ = m.value();
		}
		else
		{
			motionType_ = JPH::EMotionType::Static;
		}

		colliderParams_ = DirectX::SimpleMath::Vector4(colliderParams[0], colliderParams[1], colliderParams[2], colliderParams[3]);
		centerOfMassOffset_ = DirectX::SimpleMath::Vector3(centerOfMassOffset[0], centerOfMassOffset[1], centerOfMassOffset[2]);
		velocity_ = DirectX::SimpleMath::Vector3(velocity[0], velocity[1], velocity[2]);
		angularVelocity_ = DirectX::SimpleMath::Vector3(angularVelocity[0], angularVelocity[1], angularVelocity[2]);
	}
}
