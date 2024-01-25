#include "PhysicsActivationAdapter.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include "D3E/Components/PhysicsComponent.h"

D3E::PhysicsActivationAdapter::PhysicsActivationAdapter(entt::registry& registry, JPH::PhysicsSystem* physicsSystem) :
	registry_(registry),
	physicsSystem_(physicsSystem)
{}

void D3E::PhysicsActivationAdapter::SetActive(entt::entity entity, bool isActive)
{
	auto& physicsComponent = registry_.get<PhysicsComponent>(entity);
	registry_.patch<PhysicsComponent>(entity, [isActive](auto &component) { component.isActive_ = isActive; });
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
