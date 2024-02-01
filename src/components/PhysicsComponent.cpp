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
		std::vector<float> heightMap;
		if (heightMap_)
		{
			heightMap.reserve(heightMapSize_ * heightMapSize_);
			heightMap.insert(heightMap.begin(), &heightMap_[0], &heightMap_[heightMapSize_ * heightMapSize_]);
		}
		else
		{
			heightMap.reserve(1);
			heightMap.insert(heightMap.begin(), 0.0f);
		}

		j = json{{"type", "component"},
		         {"class", "PhysicsComponent"},
		         {"collider_type", magic_enum::enum_name(colliderType_)},
		         {"collider_params", std::vector({colliderParams_.x, colliderParams_.y, colliderParams_.z, colliderParams_.w})},
		         {"mass", mass_},
		         {"friction", friction_},
		         {"restitution", restitution_},
		         {"has_offset_center_of_mass", hasOffsetCenterOfMass_},
				 {"collider_offset", std::vector({colliderOffset_.x, colliderOffset_.y, colliderOffset_.z})},
				 {"collider_rotation", std::vector({colliderRotation_.x, colliderRotation_.y, colliderRotation_.z, colliderRotation_.w})},
		         {"center_of_mass_offset", std::vector({centerOfMassOffset_.x, centerOfMassOffset_.y, centerOfMassOffset_.z})},
		         {"motion_type", magic_enum::enum_name(motionType_)},
		         {"velocity", std::vector({velocity_.x, velocity_.y, velocity_.z})},
		         {"angular_velocity", std::vector({angularVelocity_.x, angularVelocity_.y, angularVelocity_.z})},
		         {"height_map_size", heightMapSize_},
		         {"height_map", heightMap},
		         {"is_active", isActive_}
		};
	}

	void PhysicsComponent::from_json(const json& j)
	{
		std::string colliderType;
		std::vector<float> colliderParams(4);
		std::vector<float> colliderOffset(3);
		std::vector<float> colliderRotation(4);
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
		if (j.contains("collider_offset"))
		{
			j.at("collider_offset").get_to(colliderOffset);
		}
		else
		{
			colliderOffset = { 0, 0, 0 };
		}
		if (j.contains("collider_rotation"))
		{
			j.at("collider_rotation").get_to(colliderRotation);
		}
		else
		{
			colliderOffset = { 0, 0, 0, 0 };
		}
		j.at("center_of_mass_offset").get_to(centerOfMassOffset);
		j.at("motion_type").get_to(motionType);
		j.at("velocity").get_to(velocity);
		j.at("angular_velocity").get_to(angularVelocity);
		j.at("height_map_size").get_to(heightMapSize_);
		if (j.contains("is_active"))
		{
			j.at("is_active").get_to(isActive_);
		}
		else
		{
			isActive_ = true;
		}

		if (heightMapSize_)
		{
			std::vector<float> heightMap(heightMapSize_ * heightMapSize_);
			j.at("height_map").get_to(heightMap);
			heightMap_ = new float[heightMapSize_ * heightMapSize_];
			for (size_t i = 0; i < heightMap.size(); ++i)
			{
				heightMap_[i] = heightMap[i];
			}
		}

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
		colliderOffset_ = DirectX::SimpleMath::Vector3(colliderOffset[0], colliderOffset[1], colliderOffset[2]);
		colliderRotation_ = DirectX::SimpleMath::Vector3(colliderRotation[0], colliderRotation[1], colliderRotation[2], colliderRotation[3]);
		centerOfMassOffset_ = DirectX::SimpleMath::Vector3(centerOfMassOffset[0], centerOfMassOffset[1], centerOfMassOffset[2]);
		velocity_ = DirectX::SimpleMath::Vector3(velocity[0], velocity[1], velocity[2]);
		angularVelocity_ = DirectX::SimpleMath::Vector3(angularVelocity[0], angularVelocity[1], angularVelocity[2]);
	}
}
