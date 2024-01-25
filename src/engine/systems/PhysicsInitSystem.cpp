#include "PhysicsInitSystem.h"

#include "D3E/Game.h"
#include "D3E/Components/PhysicsComponent.h"
#include "D3E/Components/TransformComponent.h"
#include "D3E/Components/render/StaticMeshComponent.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/TaperedCapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/HeightFieldShape.h>
#include <Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

#include "physics/ObjectLayers.h"

using namespace JPH;

D3E::PhysicsInitSystem::PhysicsInitSystem(entt::registry& registry, Game* game, JPH::PhysicsSystem* physicsSystem) :
	  updateObserver_{registry, entt::collector.update<PhysicsComponent>().where<TransformComponent>()},
	  game_(game),
	  physicsSystem_(physicsSystem)
{
	registry.on_construct<PhysicsComponent>()
		.connect<&PhysicsInitSystem::ComponentCreatedHandler>(
			this);

	registry.on_destroy<PhysicsComponent>()
		.connect<
			&PhysicsInitSystem::ComponentDestroyedHandler>(
			this);
}

void D3E::PhysicsInitSystem::PrePhysicsUpdate(entt::registry& reg, Game* game, float dT)
{
	auto view =
		reg.view<PhysicsComponent>();

	view.each(
		[&, game, dT](const auto entity, auto& component)
		{
			const BodyLockInterfaceLocking& lockInterface = physicsSystem_->GetBodyLockInterface();
			{
				BodyLockWrite lock(lockInterface, component.bodyID_);
				if (lock.Succeeded())
				{
					Body& body = lock.GetBody();
					body.SetMotionType(component.motionType_);
					body.SetIsSensor(component.isSensor_);
					if (component.friction_ >= 0.0f)
					{
						body.SetFriction(component.friction_);
					}
					if (component.restitution_ >= 0.0f)
					{
						body.SetRestitution(component.restitution_);
					}
					if (component.motionType_ != JPH::EMotionType::Static)
					{
						body.SetLinearVelocity(Vec3Arg(component.velocity_.x, component.velocity_.y, component.velocity_.z));
						body.SetAngularVelocity(Vec3Arg(component.angularVelocity_.x, component.angularVelocity_.y, component.angularVelocity_.z));
					}
				}
			}
		});
}

/*void D3E::PhysicsInitSystem::Update(entt::registry& reg, Game* game, float dT)
{
	updateObserver_.each(
		[&](const auto entity)
		{
			auto& component = reg.get<PhysicsComponent>(entity);
			const BodyLockInterfaceLocking& lockInterface = physicsSystem_->GetBodyLockInterface();
			{
				BodyLockWrite lock(lockInterface, component.bodyID_);
				if (lock.Succeeded())
				{
					Body& body = lock.GetBody();
					body.SetMotionType(component.motionType_);
					body.SetIsSensor(component.isSensor_);
					if (component.friction_ >= 0.0f)
					{
						body.SetFriction(component.friction_);
					}
					if (component.restitution_ >= 0.0f)
					{
						body.SetRestitution(component.restitution_);
					}
					if (component.motionType_ != JPH::EMotionType::Static)
					{
						body.SetLinearVelocity(Vec3Arg(component.velocity_.x, component.velocity_.y, component.velocity_.z));
						body.SetAngularVelocity(Vec3Arg(component.angularVelocity_.x, component.angularVelocity_.y, component.angularVelocity_.z));
					}
				}
			}
		}
		);

}*/

void D3E::PhysicsInitSystem::ComponentCreatedHandler(entt::registry& registry,
                             entt::entity entity)
{
	OnCreateComponent(registry, entity);
}

void D3E::PhysicsInitSystem::ComponentDestroyedHandler(entt::registry& registry,
                               entt::entity entity)
{
	OnDestroyComponent(registry, entity);
}

void D3E::PhysicsInitSystem::Play(entt::registry& reg, D3E::Game* game)
{
	auto view =
		reg.view<PhysicsComponent>();

	BodyInterface& bodyInterface = physicsSystem_->GetBodyInterface();
	view.each(
		[&](const auto entity, auto& physicsComponent)
		{
			bodyInterface.ActivateBody(physicsComponent.bodyID_);
		});
}

void D3E::PhysicsInitSystem::Pause(entt::registry& reg, D3E::Game* game)
{
	auto view =
		reg.view<PhysicsComponent>();

	BodyInterface& bodyInterface = physicsSystem_->GetBodyInterface();
	if (game_->IsGamePaused())
	{
		view.each(
			[&](const auto entity, auto& physicsComponent)
			{
				bodyInterface.DeactivateBody(physicsComponent.bodyID_);
			});
	}
	else
	{
		view.each(
			[&](const auto entity, auto& physicsComponent)
			{
				bodyInterface.ActivateBody(physicsComponent.bodyID_);
			});
	}

}

void D3E::PhysicsInitSystem::OnCreateComponent(entt::registry& registry, entt::entity entity)
{
	auto& physicsComponent = registry.get<PhysicsComponent>(entity);
	const auto& transformComponent = registry.get<TransformComponent>(entity);
	BodyInterface &body_interface = physicsSystem_->GetBodyInterface();
	switch (physicsComponent.colliderType_)
	{
		case SphereCollider:
		{
			if (physicsComponent.colliderParams_.x <= 0.0f)
			{
				physicsComponent.colliderParams_.x = 1.0f;
			}
			if (physicsComponent.colliderParams_.y < 0.0f)
			{
				physicsComponent.colliderParams_.y = 0.0f;
			}
			physicsComponent.collider_ = new SphereShape(physicsComponent.colliderParams_.x);
			break;
		}
		case BoxCollider:
		{
			if (physicsComponent.colliderParams_.x <= 0.0f)
			{
				physicsComponent.colliderParams_.x = 1.0f;
			}
			if (physicsComponent.colliderParams_.y <= 0.0f)
			{
				physicsComponent.colliderParams_.y = 1.0f;
			}
			if (physicsComponent.colliderParams_.z <= 0.0f)
			{
				physicsComponent.colliderParams_.z = 1.0f;
			}
			if (physicsComponent.colliderParams_.w < 0.0f)
			{
				physicsComponent.colliderParams_.w = 0.0f;
			}
			physicsComponent.collider_ = new BoxShape(Vec3Arg(physicsComponent.colliderParams_.x, physicsComponent.colliderParams_.y, physicsComponent.colliderParams_.z), physicsComponent.colliderParams_.w);
			break;
		}
		case CapsuleCollider:
		{
			if (physicsComponent.colliderParams_.x <= 0.0f)
			{
				physicsComponent.colliderParams_.x = 1.0f;
			}
			if (physicsComponent.colliderParams_.y <= 0.0f)
			{
				physicsComponent.colliderParams_.y = 1.0f;
			}
			physicsComponent.collider_ = new CapsuleShape(physicsComponent.colliderParams_.x, physicsComponent.colliderParams_.y);
			break;
		}
		case TaperedCapsuleCollider:
		{
			if (physicsComponent.colliderParams_.x <= 0.0f)
			{
				physicsComponent.colliderParams_.x = 1.0f;
			}
			if (physicsComponent.colliderParams_.y <= 0.0f)
			{
				physicsComponent.colliderParams_.y = 1.0f;
			}
			if (physicsComponent.colliderParams_.z <= 0.0f)
			{
				physicsComponent.colliderParams_.z = 1.0f;
			}
			TaperedCapsuleShapeSettings taperedCapsuleSettings(physicsComponent.colliderParams_.x, physicsComponent.colliderParams_.y, physicsComponent.colliderParams_.z);
			ShapeSettings::ShapeResult shapeResult = taperedCapsuleSettings.Create();
			physicsComponent.collider_ = new TaperedCapsuleShape(taperedCapsuleSettings, shapeResult);
			break;
		}
		case CylinderCollider:
		{
			if (physicsComponent.colliderParams_.x <= 0.0f)
			{
				physicsComponent.colliderParams_.x = 1.0f;
			}
			if (physicsComponent.colliderParams_.y <= 0.0f)
			{
				physicsComponent.colliderParams_.y = 1.0f;
			}
			if (physicsComponent.colliderParams_.z < 0.0f)
			{
				physicsComponent.colliderParams_.z = 0.0f;
			}
			physicsComponent.collider_ = new CylinderShape(physicsComponent.colliderParams_.x, physicsComponent.colliderParams_.y, physicsComponent.colliderParams_.z);
			break;
		}
		/*case ConvexHullCollider:
		{
			auto& meshComponent = registry.get<StaticMeshComponent>(entity);
			ConvexHullShapeSettings shapeSettings(meshComponent.)
		}*/
		case HeightFieldCollider:
		{
			if (physicsComponent.heightMap_)
			{
				HeightFieldShapeSettings shapeSettings(physicsComponent.heightMap_, Vec3Arg(transformComponent.position.x, transformComponent.position.y, transformComponent.position.z),
				                                       Vec3Arg(transformComponent.scale.x, transformComponent.scale.y, transformComponent.scale.z), physicsComponent.heightMapSize_);
				ShapeSettings::ShapeResult shapeResult = shapeSettings.Create();
				physicsComponent.collider_ = new HeightFieldShape(shapeSettings, shapeResult);
			}
		}
	}
	//ShapeSettings::ShapeResult shapeResult = physicsComponent.collider_->Create();
	//ShapeRefC colliderRef = shapeResult.Get();
	//*(physicsComponent.collider_) =
	if (physicsComponent.hasOffsetCenterOfMass_)
	{
		OffsetCenterOfMassShapeSettings shapeSettings(Vec3Arg(physicsComponent.centerOfMassOffset_.x, physicsComponent.centerOfMassOffset_.y, physicsComponent.centerOfMassOffset_.z), physicsComponent.collider_);
		ShapeSettings::ShapeResult shapeResult = shapeSettings.Create();
		physicsComponent.collider_ = shapeResult.Get();
	}

	ObjectLayer currentLayer;
	switch (physicsComponent.motionType_)
	{
		case JPH::EMotionType::Static:
			currentLayer = Layers::NON_MOVING;
			break;
		case JPH::EMotionType::Kinematic:
		case JPH::EMotionType::Dynamic:
			currentLayer = Layers::MOVING;
			break;
	}
	BodyCreationSettings bodySettings(physicsComponent.collider_, RVec3(transformComponent.position.x, transformComponent.position.y, transformComponent.position.z),
	                                  Quat(transformComponent.rotation.x, transformComponent.rotation.y, transformComponent.rotation.z, transformComponent.rotation.w),
	                                  physicsComponent.motionType_, currentLayer);
	if (physicsComponent.mass_ > 0.0f)
	{
		bodySettings.mMassPropertiesOverride.ScaleToMass(physicsComponent.mass_);
		bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
	}
	if (physicsComponent.friction_ >= 0.0f)
	{
		bodySettings.mFriction = physicsComponent.friction_;
	}
	if (physicsComponent.restitution_ >= 0.0f)
	{
		bodySettings.mRestitution = physicsComponent.restitution_;
	}
	if (physicsComponent.motionType_ != JPH::EMotionType::Static)
	{
		bodySettings.mLinearVelocity = Vec3Arg(physicsComponent.velocity_.x, physicsComponent.velocity_.y, physicsComponent.velocity_.z);
		bodySettings.mAngularVelocity = Vec3Arg(physicsComponent.angularVelocity_.x, physicsComponent.angularVelocity_.y, physicsComponent.angularVelocity_.z);
	}

	bodySettings.mIsSensor = physicsComponent.isSensor_;

	if (game_->IsGameRunning())
	{
		physicsComponent.bodyID_ = body_interface.CreateAndAddBody(bodySettings, EActivation::Activate);
	}
	else
	{
		physicsComponent.bodyID_ = body_interface.CreateAndAddBody(bodySettings, EActivation::DontActivate);
	}

	physicsComponent.isInitialized_ = true;
}

void D3E::PhysicsInitSystem::OnDestroyComponent(entt::registry& registry, entt::entity entity)
{
	BodyInterface& bodyInterface = physicsSystem_->GetBodyInterface();
	auto& physicsComponent = registry.get<PhysicsComponent>(entity);
	if (physicsComponent.colliderType_ == HeightFieldCollider)
	{
		delete[] physicsComponent.heightMap_;
	}
	bodyInterface.RemoveBody(physicsComponent.bodyID_);
	bodyInterface.DestroyBody(physicsComponent.bodyID_);
	//delete physicsComponent.collider_;
}
