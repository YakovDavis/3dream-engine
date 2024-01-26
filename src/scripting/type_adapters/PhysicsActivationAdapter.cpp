#include "PhysicsActivationAdapter.h"

#include "D3E/Components/PhysicsComponent.h"
#include "PhysicsActivationAdapter.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/PhysicsSystem.h>

D3E::PhysicsActivationAdapter::PhysicsActivationAdapter(
	entt::registry& registry, JPH::PhysicsSystem* physicsSystem)
	: registry_(registry), physicsSystem_(physicsSystem)
{
}

void D3E::PhysicsActivationAdapter::SetActive(entt::entity entity,
                                              bool isActive)
{
	auto& physicsComponent = registry_.get<PhysicsComponent>(entity);
	registry_.patch<PhysicsComponent>(entity, [isActive](auto& component)
	                                  { component.isActive_ = isActive; });
	JPH::BodyInterface& bodyInterface = physicsSystem_->GetBodyInterface();
	if (physicsComponent.isActive_)
	{
		bodyInterface.ActivateBody(physicsComponent.bodyID_);
	}
	else
	{
		bodyInterface.DeactivateBody(physicsComponent.bodyID_);
	}
}

void D3E::PhysicsActivationAdapter::SetTrigger(entt::entity entity,
                                               bool isSensor)
{
	auto pc = registry_.try_get<PhysicsComponent>(entity);

	if (!pc)
	{
		return;
	}

	registry_.patch<PhysicsComponent>(entity, [isSensor](auto& component)
	                                  { component.isSensor_ = isSensor; });

	const JPH::BodyLockInterfaceLocking& lockInterface =
		physicsSystem_->GetBodyLockInterface();
	{
		JPH::BodyLockWrite lock(lockInterface, pc->bodyID_);

		if (lock.Succeeded())
		{
			JPH::Body& body = lock.GetBody();
			body.SetIsSensor(isSensor);
		}
	}
}
