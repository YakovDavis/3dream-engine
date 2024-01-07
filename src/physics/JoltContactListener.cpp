#include "JoltContactListener.h"

#include "D3E/Components/PhysicsCharacterComponent.h"
#include "D3E/Components/PhysicsComponent.h"
#include "D3E/Game.h"
#include "utils/ECSUtils.h"

#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <iostream>

D3E::JoltContactListener::JoltContactListener(Game* g)
	: next_(nullptr), game_(g), ContactListener()
{
}

ValidateResult D3E::JoltContactListener::OnContactValidate(
	const Body& inBody1, const Body& inBody2, RVec3Arg inBaseOffset,
	const CollideShapeResult& inCollisionResult)
{
	if (!inBody1.IsDynamic() && !inBody1.IsSensor() && !inBody2.IsSensor())
		JPH_BREAKPOINT;

	ValidateResult result;
	if (next_ != nullptr)
		result = next_->OnContactValidate(inBody1, inBody2, inBaseOffset,
		                                  inCollisionResult);
	else
		result = ContactListener::OnContactValidate(
			inBody1, inBody2, inBaseOffset, inCollisionResult);

	RVec3 contact_point = inBaseOffset + inCollisionResult.mContactPointOn1;
	// DebugRenderer::sInstance->DrawArrow(contact_point, contact_point -
	// inCollisionResult.mPenetrationAxis.NormalizedOr(Vec3::sZero()),
	// Color::sBlue, 0.05f);

	// Trace("Validate %u and %u result %d", inBody1.GetID().GetIndex(),
	// inBody2.GetID().GetIndex(), (int)result);

	return result;
}

void D3E::JoltContactListener::OnContactAdded(const Body& inBody1,
                                              const Body& inBody2,
                                              const ContactManifold& inManifold,
                                              ContactSettings& ioSettings)
{
	// Expect bodies to be sorted
	if (!(inBody1.GetID() < inBody2.GetID()))
		JPH_BREAKPOINT;

	// Trace("Contact added %u (%08x) and %u (%08x)",
	// inBody1.GetID().GetIndex(), inManifold.mSubShapeID1.GetValue(),
	// inBody2.GetID().GetIndex(), inManifold.mSubShapeID2.GetValue());

	// Insert new manifold into state map
	{
		lock_guard lock(stateMutex_);
		SubShapeIDPair key(inBody1.GetID(), inManifold.mSubShapeID1,
		                   inBody2.GetID(), inManifold.mSubShapeID2);
		if (state_.find(key) != state_.end())
			JPH_BREAKPOINT; // Added contact that already existed
		state_[key] = StatePair(inManifold.mBaseOffset,
		                        inManifold.mRelativeContactPointsOn1);
	}

	{
		lock_guard lock(luaMutex_);
		HandleBodyCollision(inBody1.GetID(), inBody2.GetID(),
		                    &D3E::ScriptComponent::OnCollisionEnter);
	}

	if (next_ != nullptr)
		next_->OnContactAdded(inBody1, inBody2, inManifold, ioSettings);
}

void D3E::JoltContactListener::OnContactPersisted(
	const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold,
	ContactSettings& ioSettings)
{
	// Expect bodies to be sorted
	if (!(inBody1.GetID() < inBody2.GetID()))
		JPH_BREAKPOINT;

	// Trace("Contact persisted %u (%08x) and %u (%08x)",
	// inBody1.GetID().GetIndex(), inManifold.mSubShapeID1.GetValue(),
	// inBody2.GetID().GetIndex(), inManifold.mSubShapeID2.GetValue());

	// Update existing manifold in state map
	{
		lock_guard lock(stateMutex_);
		SubShapeIDPair key(inBody1.GetID(), inManifold.mSubShapeID1,
		                   inBody2.GetID(), inManifold.mSubShapeID2);
		StateMap::iterator i = state_.find(key);
		if (i != state_.end())
			i->second = StatePair(inManifold.mBaseOffset,
			                      inManifold.mRelativeContactPointsOn1);
		else
			JPH_BREAKPOINT; // Persisted contact that didn't exist
	}

	{
		lock_guard lock(luaMutex_);
		HandleBodyCollision(inBody1.GetID(), inBody2.GetID(),
		                    &D3E::ScriptComponent::OnCollisionStay);
	}

	if (next_ != nullptr)
		next_->OnContactPersisted(inBody1, inBody2, inManifold, ioSettings);
}

void D3E::JoltContactListener::OnContactRemoved(
	const SubShapeIDPair& inSubShapePair)
{
	// Expect bodies to be sorted
	if (!(inSubShapePair.GetBody1ID() < inSubShapePair.GetBody2ID()))
		JPH_BREAKPOINT;

	// Trace("Contact removed %u (%08x) and %u (%08x)",
	// inSubShapePair.GetBody1ID().GetIndex(),
	// inSubShapePair.GetSubShapeID1().GetValue(),
	// inSubShapePair.GetBody2ID().GetIndex(),
	// inSubShapePair.GetSubShapeID2().GetValue());

	// Update existing manifold in state map
	{
		lock_guard lock(stateMutex_);
		StateMap::iterator i = state_.find(inSubShapePair);
		if (i != state_.end())
			state_.erase(i);
		else
			JPH_BREAKPOINT; // Removed contact that didn't exist
	}

	{
		lock_guard lock(luaMutex_);
		HandleBodyCollision(inSubShapePair,
		                    &D3E::ScriptComponent::OnCollisionExit);
	}

	if (next_ != nullptr)
		next_->OnContactRemoved(inSubShapePair);
}

void D3E::JoltContactListener::HandleBodyCollision(
	const BodyID& bodyId1, const BodyID& bodyId2,
	void (D3E::ScriptComponent::*handler)(const JPH::BodyID&))
{
	RaiseCollisionEvent(bodyId1, bodyId2, handler);
	RaiseCollisionEvent(bodyId2, bodyId1, handler);
}

void D3E::JoltContactListener::HandleBodyCollision(
	const SubShapeIDPair& inSubShapePair,
	void (D3E::ScriptComponent::*handler)(const JPH::BodyID&))
{
	const auto& bodyId1 = inSubShapePair.GetBody1ID();
	const auto& bodyId2 = inSubShapePair.GetBody2ID();

	HandleBodyCollision(bodyId1, bodyId2, handler);
}

void D3E::JoltContactListener::RaiseCollisionEvent(
	const BodyID& firstBodyId, const BodyID& secondBodyId,
	void (D3E::ScriptComponent::*handler)(const JPH::BodyID&))
{
	auto& registry = game_->GetRegistry();

	auto e =
		ECSUtils::GetEntityByBodyId<PhysicsComponent>(registry, firstBodyId);

	//  Dirty as fuck workaround to handle case with PhysicsCharacterComponent
	if (!e)
	{
		e = ECSUtils::GetEntityByBodyId<PhysicsCharacterComponent>(registry,
		                                                           firstBodyId);
	}

	if (!e)
	{
		Debug::LogError("[JoltContactListener] OnContactAdded() : Entity not "
		                "found for body1");

		return;
	}

	auto sc1 = ECSUtils::TryGet<ScriptComponent>(registry, e.value());

	if (!sc1)
		return;

	(sc1->*handler)(secondBodyId);
}
