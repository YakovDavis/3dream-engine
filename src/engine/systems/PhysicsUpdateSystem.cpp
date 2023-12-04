#include "PhysicsUpdateSystem.h"

#include "D3E/Components/PhysicsComponent.h"
#include "D3E/Components/TransformComponent.h"

#include <Jolt/Physics/PhysicsSystem.h>

using namespace JPH;

D3E::PhysicsUpdateSystem::PhysicsUpdateSystem(JPH::PhysicsSystem* physicsSystem) :
	  physicsSystem_(physicsSystem)
{}

void D3E::PhysicsUpdateSystem::Update(entt::registry& reg, Game* game, float dT)
{
	auto view =
		reg.view<PhysicsComponent, TransformComponent>();

	view.each(
		[&, game, dT](const auto entity, auto& physC, auto& transformC)
		{
			const BodyLockInterfaceLocking& lockInterface = physicsSystem_->GetBodyLockInterface();
			{
				BodyLockRead lock(lockInterface, physC.bodyID_);
				if (lock.Succeeded())
				{
					const Body& body = lock.GetBody();
					reg.patch<TransformComponent>(
						entity, [&](auto& transformComponent)
						{
							transformComponent.position.x = body.GetPosition().GetX();
							transformComponent.position.y = body.GetPosition().GetY();
							transformComponent.position.z = body.GetPosition().GetZ();
							transformComponent.rotation.x = body.GetRotation().GetX();
							transformComponent.rotation.y = body.GetRotation().GetY();
							transformComponent.rotation.z = body.GetRotation().GetZ();
						});

					reg.patch<PhysicsComponent>(
						entity, [&](auto& physicsComponent)
						{
							physicsComponent.velocity_.x = body.GetLinearVelocity().GetX();
							physicsComponent.velocity_.y = body.GetLinearVelocity().GetY();
							physicsComponent.velocity_.z = body.GetLinearVelocity().GetZ();
							physicsComponent.angularVelocity_.x = body.GetAngularVelocity().GetX();
							physicsComponent.angularVelocity_.y = body.GetAngularVelocity().GetY();
							physicsComponent.angularVelocity_.z = body.GetAngularVelocity().GetZ();
						});
				}
			}
		});
}
