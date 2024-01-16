#include "CharacterInitSystem.h"

#include "D3E/Components/PhysicsComponent.h"
#include "D3E/Components/PhysicsCharacterComponent.h"
#include "D3E/Components/TransformComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Game.h"
#include "D3E/Debug.h"


#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/TaperedCapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/HeightFieldShape.h>
#include <Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.h>

#include "input/InputDevice.h"
#include "input/Keys.h"

#include "physics/ObjectLayers.h"

#include "SimpleMath.h"

#include <iostream>

using namespace JPH;

D3E::CharacterInitSystem::CharacterInitSystem(entt::registry& registry, JPH::PhysicsSystem* physicsSystem) :
	  updateObserver_{registry, entt::collector.update<PhysicsCharacterComponent>().where<PhysicsComponent>()},
	  physicsSystem_(physicsSystem)
{
	registry.on_construct<PhysicsCharacterComponent>()
		.connect<&CharacterInitSystem::ComponentCreatedHandler>(
			this);

}

void D3E::CharacterInitSystem::Update(entt::registry& reg, Game* game, float dT)
{
	updateObserver_.each(
		[&](const auto entity)
		{
			auto& component = reg.get<PhysicsCharacterComponent>(entity);
			const BodyLockInterfaceLocking& lockInterface = physicsSystem_->GetBodyLockInterface();
			{
				BodyLockWrite lock(lockInterface, component.bodyID_);
				if (lock.Succeeded())
				{
					Body& body = lock.GetBody();
					if (component.friction_ >= 0.0f)
					{
						body.SetFriction(component.friction_);
					}
					if (component.restitution_ >= 0.0f)
					{
						body.SetRestitution(component.restitution_);
					}
					body.SetLinearVelocity(Vec3Arg(component.velocity_.x, component.velocity_.y, component.velocity_.z));
					body.SetAngularVelocity(Vec3Arg(component.angularVelocity_.x, component.angularVelocity_.y, component.angularVelocity_.z));
				}
			}
			while (component.yaw_ < -DirectX::XM_2PI)
				component.yaw_ += DirectX::XM_2PI;
			while (component.yaw_ > DirectX::XM_2PI)
				component.yaw_ -= DirectX::XM_2PI;
			while (component.pitch_ < -DirectX::XM_2PI)
				component.pitch_ += DirectX::XM_2PI;
			while (component.pitch_ > DirectX::XM_2PI)
				component.pitch_ -= DirectX::XM_2PI;
		}
	);

}

void D3E::CharacterInitSystem::PrePhysicsUpdate(entt::registry& reg, Game* game, float dT)
{
	auto view =
		reg.view<TransformComponent, CameraComponent, PhysicsCharacterComponent>();
	if (view.begin() == view.end())
	{
		//Debug::LogError("Character controller entity not found");
		return;
	}
	auto characterController = view.front();

	auto [transform, camera, character] = view.get<TransformComponent, CameraComponent, PhysicsCharacterComponent>(characterController);

	// Update scene time
	character.time_ += dT;

	if (game->GetInputDevice()->IsKeyDown(Keys::LeftButton) ||
	    !character.isLMBActivated_)
	{
		character.yaw_ +=
			character.sensitivityX_ * game->GetInputDevice()->MouseOffsetInTick.x;
		while (character.yaw_ < -DirectX::XM_2PI)
			character.yaw_ += DirectX::XM_2PI;
		while (character.yaw_ > DirectX::XM_2PI)
			character.yaw_ -= DirectX::XM_2PI;
		character.pitch_ -=
			character.sensitivityY_ * game->GetInputDevice()->MouseOffsetInTick.y;
		while (character.pitch_ < -DirectX::XM_2PI)
			character.pitch_ += DirectX::XM_2PI;
		while (character.pitch_ > DirectX::XM_2PI)
			character.pitch_ -= DirectX::XM_2PI;
	}

	camera.up = Vector3::Transform(Vector3::Up, DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(character.yaw_, character.pitch_, 0.0f));
	camera.forward = Vector3::Transform(Vector3::Forward, DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(character.yaw_, character.pitch_, 0.0f));
	camera.offset = Vector3::Transform(camera.initialOffset, DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(character.yaw_, 0.0f, 0.0f));

	// Determine controller input
	Vec3 control_input = Vec3::sZero();
	if (game->GetInputDevice()->IsKeyDown(Keys::A))		control_input.SetZ(1);
	if (game->GetInputDevice()->IsKeyDown(Keys::D))	control_input.SetZ(-1);
	if (game->GetInputDevice()->IsKeyDown(Keys::W))		control_input.SetX(1);
	if (game->GetInputDevice()->IsKeyDown(Keys::S))		control_input.SetX(-1);
	if (control_input != Vec3::sZero())
		control_input = control_input.Normalized();
	//std::cout << control_input << "\n";

	// Rotate controls to align with the camera
	Vec3 cam_fwd = Vec3(camera.forward.x, 0.0f, camera.forward.z);
	cam_fwd = cam_fwd.NormalizedOr(Vec3::sAxisX());
	Quat rotation = Quat::sFromTo(Vec3::sAxisX(), cam_fwd);
	control_input = rotation * control_input;
	//std::cout << control_input << "\n";

	// Check actions
	bool jump = false;
	bool switch_stance = false;
	if (game->GetInputDevice()->IsKeyDown(Keys::LeftControl))
		switch_stance = true;
	else if (game->GetInputDevice()->IsKeyDown(Keys::LeftShift))
		jump = true;

	Character::EGroundState ground_state = character.character_->GetGroundState();
	if (ground_state == Character::EGroundState::OnSteepGround
	    || ground_state == Character::EGroundState::NotSupported)
	{
		Vec3 normal = character.character_->GetGroundNormal();
		normal.SetY(0.0f);
		float dot = normal.Dot(control_input);
		if (dot < 0.0f)
			control_input -= (dot * normal) / normal.LengthSq();
	}
	//std::cout << control_input << "\n";

	if (character.controlMovementDuringJump_ || character.character_->IsSupported())
	{
		// Update velocity
		Vec3 current_velocity = character.character_->GetLinearVelocity();
		Vec3 desired_velocity = character.speed_ * control_input;
		//std::cout << current_velocity << " " << desired_velocity << "\n";
		desired_velocity.SetY(current_velocity.GetY());
		Vec3 new_velocity = 0.75f * current_velocity + 0.25f * desired_velocity;
		//std::cout << new_velocity << "\n";

		// Jump
		if (jump && ground_state == Character::EGroundState::OnGround)
			new_velocity += Vec3(0, character.jumpSpeed_, 0);

		// Update the velocity
		character.character_->SetLinearVelocity(new_velocity);
		//std::cout << new_velocity << "\n";

		transform.position.x += new_velocity.GetX() * dT;
		transform.position.y += new_velocity.GetY() * dT;
		transform.position.z += new_velocity.GetZ() * dT;

		//camera.offset = transform.position;

		transform.rotation.x = rotation.GetX();
		transform.rotation.y = rotation.GetY();
		transform.rotation.z = rotation.GetZ();
		transform.rotation.w = rotation.GetW();
	}
}

void D3E::CharacterInitSystem::PostPhysicsUpdate(entt::registry& reg)
{
	auto view =
		reg.view<PhysicsCharacterComponent, CameraComponent, TransformComponent>();
	if (view.begin() == view.end())
	{
		//Debug::LogError("Character controller entity not found");
		return;
	}
	auto characterController = view.front();

	auto character = view.get<PhysicsCharacterComponent>(characterController);

	character.character_->PostSimulation(character.collisionTolerance_);

	auto camera = view.get<CameraComponent>(characterController);
	RVec3 characterPosition;
	Quat characterRotation;
	character.character_->GetPositionAndRotation(characterPosition,characterRotation);
	//camera.offset = DirectX::SimpleMath::Vector3(characterPosition.GetX(), characterPosition.GetY(), characterPosition.GetZ());
	auto transform = view.get<TransformComponent>(characterController);
	transform.position = camera.offset;
}

void D3E::CharacterInitSystem::ComponentCreatedHandler(entt::registry& registry,
                                                     entt::entity entity)
{
	auto& characterComponent = registry.get<PhysicsCharacterComponent>(entity);
	const auto& transformComponent = registry.get<TransformComponent>(entity);
	BodyInterface &body_interface = physicsSystem_->GetBodyInterface();
	switch (characterComponent.colliderType_)
	{
		case SphereCollider:
		{
			if (characterComponent.colliderParams_.x <= 0.0f)
			{
				characterComponent.colliderParams_.x = 1.0f;
			}
			characterComponent.collider_ = new SphereShape(characterComponent.colliderParams_.x);
			break;
		}
		case BoxCollider:
		{
			if (characterComponent.colliderParams_.x <= 0.0f)
			{
				characterComponent.colliderParams_.x = 1.0f;
			}
			if (characterComponent.colliderParams_.y <= 0.0f)
			{
				characterComponent.colliderParams_.y = 1.0f;
			}
			if (characterComponent.colliderParams_.z <= 0.0f)
			{
				characterComponent.colliderParams_.z = 1.0f;
			}
			if (characterComponent.colliderParams_.w < 0.0f)
			{
				characterComponent.colliderParams_.w = 0.0f;
			}
			characterComponent.collider_ = new BoxShape(Vec3Arg(characterComponent.colliderParams_.x, characterComponent.colliderParams_.y, characterComponent.colliderParams_.z), characterComponent.colliderParams_.w);
			break;
		}
		case CapsuleCollider:
		{
			if (characterComponent.colliderParams_.x <= 0.0f)
			{
				characterComponent.colliderParams_.x = 1.0f;
			}
			if (characterComponent.colliderParams_.y <= 0.0f)
			{
				characterComponent.colliderParams_.y = 1.0f;
			}
			characterComponent.collider_ = new CapsuleShape(characterComponent.colliderParams_.x, characterComponent.colliderParams_.y);
			break;
		}
		case TaperedCapsuleCollider:
		{
			if (characterComponent.colliderParams_.x <= 0.0f)
			{
				characterComponent.colliderParams_.x = 1.0f;
			}
			if (characterComponent.colliderParams_.y <= 0.0f)
			{
				characterComponent.colliderParams_.y = 1.0f;
			}
			if (characterComponent.colliderParams_.z <= 0.0f)
			{
				characterComponent.colliderParams_.z = 1.0f;
			}
			TaperedCapsuleShapeSettings taperedCapsuleSettings(characterComponent.colliderParams_.x, characterComponent.colliderParams_.y, characterComponent.colliderParams_.z);
			ShapeSettings::ShapeResult shapeResult = taperedCapsuleSettings.Create();
			characterComponent.collider_ = new TaperedCapsuleShape(taperedCapsuleSettings, shapeResult);
			break;
		}
		case CylinderCollider:
		{
			if (characterComponent.colliderParams_.x <= 0.0f)
			{
				characterComponent.colliderParams_.x = 1.0f;
			}
			if (characterComponent.colliderParams_.y <= 0.0f)
			{
				characterComponent.colliderParams_.y = 1.0f;
			}
			if (characterComponent.colliderParams_.z < 0.0f)
			{
				characterComponent.colliderParams_.z = 0.0f;
			}
			characterComponent.collider_ = new CylinderShape(characterComponent.colliderParams_.x, characterComponent.colliderParams_.y, characterComponent.colliderParams_.z);
			break;
		}
			/*case ConvexHullCollider:
			{
			    auto& meshComponent = registry.get<StaticMeshComponent>(entity);
			    ConvexHullShapeSettings shapeSettings(meshComponent.)
			}*/
			/*case HeightFieldCollider:
			{
			    if (physicsComponent.heightMap_)
			    {
			        HeightFieldShapeSettings shapeSettings(physicsComponent.heightMap_, Vec3Arg(transformComponent.position.x, transformComponent.position.y, transformComponent.position.z),
			                                               Vec3Arg(transformComponent.scale.x, transformComponent.scale.y, transformComponent.scale.z), physicsComponent.heightMapSize_);
			        ShapeSettings::ShapeResult shapeResult = shapeSettings.Create();
			        physicsComponent.collider_ = new HeightFieldShape(shapeSettings, shapeResult);
			    }
			}*/
	}
	//ShapeSettings::ShapeResult shapeResult = physicsComponent.collider_->Create();
	//ShapeRefC colliderRef = shapeResult.Get();
	//*(physicsComponent.collider_) =
	if (characterComponent.hasOffsetCenterOfMass_)
	{
		OffsetCenterOfMassShapeSettings shapeSettings(Vec3Arg(characterComponent.centerOfMassOffset_.x, characterComponent.centerOfMassOffset_.y, characterComponent.centerOfMassOffset_.z), characterComponent.collider_);
		ShapeSettings::ShapeResult shapeResult = shapeSettings.Create();
		characterComponent.collider_ = shapeResult.Get();
	}

	/*switch (physicsComponent.colliderType_)
	{
		case CapsuleCollider:
			characterComponent.standingShape_ = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * cCharacterHeightStanding + cCharacterRadiusStanding, 0), Quat::sIdentity(), new CapsuleShape(0.5f * cCharacterHeightStanding, cCharacterRadiusStanding)).Create().Get();
			mCrouchingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * cCharacterHeightCrouching + cCharacterRadiusCrouching, 0), Quat::sIdentity(), new CapsuleShape(0.5f * cCharacterHeightCrouching, cCharacterRadiusCrouching)).Create().Get();
			break;

		case CylinderCollider:
			mStandingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * cCharacterHeightStanding + cCharacterRadiusStanding, 0), Quat::sIdentity(), new CylinderShape(0.5f * cCharacterHeightStanding + cCharacterRadiusStanding, cCharacterRadiusStanding)).Create().Get();
			mCrouchingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * cCharacterHeightCrouching + cCharacterRadiusCrouching, 0), Quat::sIdentity(), new CylinderShape(0.5f * cCharacterHeightCrouching + cCharacterRadiusCrouching, cCharacterRadiusCrouching)).Create().Get();
			break;

		case EType::Box:
			mStandingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * cCharacterHeightStanding + cCharacterRadiusStanding, 0), Quat::sIdentity(), new BoxShape(Vec3(cCharacterRadiusStanding, 0.5f * cCharacterHeightStanding + cCharacterRadiusStanding, cCharacterRadiusStanding))).Create().Get();
			mCrouchingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * cCharacterHeightCrouching + cCharacterRadiusCrouching, 0), Quat::sIdentity(), new BoxShape(Vec3(cCharacterRadiusCrouching, 0.5f * cCharacterHeightCrouching + cCharacterRadiusCrouching, cCharacterRadiusCrouching))).Create().Get();
			break;
	}*/
	Ref<CharacterSettings> settings = new CharacterSettings();
	settings->mMaxSlopeAngle = characterComponent.maxSlopeAngle_;
	settings->mLayer = Layers::MOVING;
	settings->mShape = characterComponent.collider_; //???
	settings->mFriction = characterComponent.friction_;
	settings->mSupportingVolume = characterComponent.supportingVolume_;
	characterComponent.character_ = new Character(settings, RVec3(transformComponent.position.x, transformComponent.position.y, transformComponent.position.z),
	                           Quat(transformComponent.rotation.x, transformComponent.rotation.y, transformComponent.rotation.z, transformComponent.rotation.w), 0, physicsSystem_);
	//characterComponent.character_ = new Character(settings, RVec3::sZero(), Quat::sIdentity(), 0, physicsSystem_);
	characterComponent.character_->AddToPhysicsSystem(EActivation::Activate);
	characterComponent.bodyID_ = characterComponent.character_->GetBodyID();
}
