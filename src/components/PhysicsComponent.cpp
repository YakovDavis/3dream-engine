#include "D3E/Components/PhysicsComponent.h"
#include "core/magic_enum.hpp"
#include "json.hpp"

namespace D3E
{
	void to_json(json& j, const PhysicsComponent& t)
	{
		j = json{{"type", "component"},
		         {"class", "PhysicsComponent"},
		         {"collider_type", magic_enum::enum_name(t.colliderType_)},
		         {"collider_params", std::vector({t.colliderParams_.x, t.colliderParams_.y, t.colliderParams_.z, t.colliderParams_.w})},
		         {"mass", t.mass_},
		         {"friction", t.friction_},
		         {"restitution", t.restitution_},
		         {"has_offset_center_of_mass", t.hasOffsetCenterOfMass_},
		         {"center_of_mass_offset", std::vector({t.centerOfMassOffset_.x, t.centerOfMassOffset_.y, t.centerOfMassOffset_.z})},
		         {"motion_type", magic_enum::enum_name(t.motionType_)},
		         {"velocity", std::vector({t.velocity_.x, t.velocity_.y, t.velocity_.z})},
		         {"angular_velocity", std::vector({t.angularVelocity_.x, t.angularVelocity_.y, t.angularVelocity_.z})}
		};
	}

	void from_json(const json& j, PhysicsComponent& t)
	{
		std::string colliderType;
		std::vector<float> colliderParams(4);
		std::vector<float> centerOfMassOffset(3);
		std::string motionType;
		std::vector<float> velocity(3);
		std::vector<float> angularVelocity(3);

		j.at("collider_type").get_to(colliderType);
		j.at("collider_params").get_to(colliderParams);
		j.at("mass").get_to(t.mass_);
		j.at("friction").get_to(t.friction_);
		j.at("restitution").get_to(t.restitution_);
		j.at("has_offset_center_of_mass").get_to(t.hasOffsetCenterOfMass_);
		j.at("center_of_mass_offset").get_to(centerOfMassOffset);
		j.at("motion_type").get_to(motionType);
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

		auto m = magic_enum::enum_cast<JPH::EMotionType>(motionType);
		if (m.has_value())
		{
			t.motionType_ = m.value();
		}
		else
		{
			t.motionType_ = JPH::EMotionType::Static;
		}

		t.colliderParams_ = DirectX::SimpleMath::Vector4(colliderParams[0], colliderParams[1], colliderParams[2], colliderParams[3]);
		t.centerOfMassOffset_ = DirectX::SimpleMath::Vector3(centerOfMassOffset[0], centerOfMassOffset[1], centerOfMassOffset[2]);
		t.velocity_ = DirectX::SimpleMath::Vector3(velocity[0], velocity[1], velocity[2]);
		t.angularVelocity_ = DirectX::SimpleMath::Vector3(angularVelocity[0], angularVelocity[1], angularVelocity[2]);
	}
}
