#pragma once

#include "D3E/CommonHeader.h"
#include "BaseComponent.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

#include "SimpleMath.h"

namespace D3E
{
	enum ColliderType
	{
		SphereCollider,
		BoxCollider,
		CapsuleCollider,
		TaperedCapsuleCollider,
		CylinderCollider,
		ConvexHullCollider,
		MeshCollider,
		HeightFieldCollider
	};

	struct D3EAPI PhysicsComponent : public BaseComponent
	{
		bool isInitialized_ = false;

		JPH::BodyID bodyID_;
		JPH::Ref<JPH::Shape> collider_;
		ColliderType colliderType_ = SphereCollider;
		DirectX::SimpleMath::Vector4 colliderParams_;
		float mass_ = 0.0f;
		float friction_ = 0.2f;
		float restitution_ = 0.0f;
		bool hasOffsetCenterOfMass_ = false;
		bool isSensor_ = false;
		DirectX::SimpleMath::Vector3 colliderOffset_ = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
		DirectX::SimpleMath::Vector4 colliderRotation_ = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
		DirectX::SimpleMath::Vector3 centerOfMassOffset_;
		JPH::EMotionType motionType_ = JPH::EMotionType::Static;
		DirectX::SimpleMath::Vector3 velocity_;
		DirectX::SimpleMath::Vector3 angularVelocity_;
		int heightMapSize_ = 0;
		float* heightMap_ = nullptr;
		bool isActive_ = true;

		void to_json(json& j) const override;
		void from_json(const json& j) override;
	};

	void to_json(json& j, const PhysicsComponent& t);

	void from_json(const json& j, PhysicsComponent& t);
}
