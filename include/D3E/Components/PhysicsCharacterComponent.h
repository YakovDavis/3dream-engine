#ifndef D3ENGINE_PHYSICSCHARACTERCOMPONENT_H
#define D3ENGINE_PHYSICSCHARACTERCOMPONENT_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Geometry/Plane.h>

namespace D3E
{
	struct PhysicsCharacterComponent
	{
		JPH::Ref<JPH::Character> character_;
		float maxSlopeAngle_ = JPH::DegreesToRadians(50.0f);
		JPH::Plane supportingVolume_ = JPH::Plane( JPH::Vec3::sAxisY(), -1.0e10f);
		JPH::RefConst<JPH::Shape> standingShape_;
		JPH::RefConst<JPH::Shape> crouchingShape_;
		float collisionTolerance_ = 0.05f;
		float time_ = 0.0f;
		bool controlMovementDuringJump_ = true;
		float yaw_ = 3.14f;
		float pitch_ = 0.0f;
		float speed_ = 0.02f;
		float jumpSpeed_ = 0.02f;
		float sensitivityX_ = 0.01f;
		float sensitivityY_ = 0.01f;
		bool isLMBActivated_ = true;
	};
}

#endif // D3ENGINE_PHYSICSCHARACTERCOMPONENT_H
