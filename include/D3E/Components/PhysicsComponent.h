#ifndef D3ENGINE_PHYSICSCOMPONENT_H
#define D3ENGINE_PHYSICSCOMPONENT_H

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

	struct PhysicsComponent
	{
		bool isInitialized_ = false;
		JPH::PhysicsSystem* systemRef_;

		JPH::BodyID bodyID_;
		JPH::Ref<JPH::Shape> collider_ = nullptr;
		ColliderType colliderType_;
		DirectX::SimpleMath::Vector4 colliderParams_;
		float mass_;
		float friction_;
		float restitution_;
		bool hasOffsetCenterOfMass_ = false;
		DirectX::SimpleMath::Vector3 centerOfMassOffset_;
		JPH::EMotionType motionType_;
		DirectX::SimpleMath::Vector3 velocity_;
		DirectX::SimpleMath::Vector3 angularVelocity_;
	};
}


#endif // D3ENGINE_PHYSICSCOMPONENT_H
