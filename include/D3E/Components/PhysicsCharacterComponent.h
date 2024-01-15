#pragma once

#include "D3E/CommonHeader.h"
#include "D3E/Components/PhysicsComponent.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Geometry/Plane.h>

namespace D3E
{
	struct PhysicsCharacterComponent : public BaseComponent
	{
		JPH::Ref<JPH::Character> character_;
		float maxSlopeAngle_ = JPH::DegreesToRadians(50.0f);
		JPH::Plane supportingVolume_ = JPH::Plane( JPH::Vec3::sAxisY(), -0.3);
		float collisionTolerance_ = 0.05f;
		float time_ = 0.0f;
		bool controlMovementDuringJump_ = true;
		float yaw_ = 0.0f;
		float pitch_ = 0.0f;
		float speed_ = 30.0f;
		float jumpSpeed_ = 10.0f;
		float sensitivityX_ = 0.01f;
		float sensitivityY_ = 0.01f;
		bool isLMBActivated_ = true;
		JPH::BodyID bodyID_;
		JPH::Ref<JPH::Shape> collider_;
		ColliderType colliderType_;
		DirectX::SimpleMath::Vector4 colliderParams_;
		float mass_ = 0.0f;
		float friction_ = 0.2f;
		float restitution_ = 0.0f;
		bool hasOffsetCenterOfMass_ = false;
		DirectX::SimpleMath::Vector3 centerOfMassOffset_;
		DirectX::SimpleMath::Vector3 velocity_;
		DirectX::SimpleMath::Vector3 angularVelocity_;

		void to_json(json& j) const override;
		void from_json(const json& j) override;
	};

	void to_json(json& j, const PhysicsCharacterComponent& t);

	void from_json(const json& j, PhysicsCharacterComponent& t);
}
