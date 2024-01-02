#include "JoltContactListener.h"

#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Collision/CollideShape.h>

#include <iostream>

ValidateResult D3E::JoltContactListener::OnContactValidate(const Body &inBody1, const Body &inBody2, RVec3Arg inBaseOffset, const CollideShapeResult &inCollisionResult)
{
	if (!inBody1.IsDynamic() && !inBody1.IsSensor() && !inBody2.IsSensor())
		JPH_BREAKPOINT;

	ValidateResult result;
	if (next_ != nullptr)
		result = next_->OnContactValidate(inBody1, inBody2, inBaseOffset, inCollisionResult);
	else
		result = ContactListener::OnContactValidate(inBody1, inBody2, inBaseOffset, inCollisionResult);

	RVec3 contact_point = inBaseOffset + inCollisionResult.mContactPointOn1;
	//DebugRenderer::sInstance->DrawArrow(contact_point, contact_point - inCollisionResult.mPenetrationAxis.NormalizedOr(Vec3::sZero()), Color::sBlue, 0.05f);

	//Trace("Validate %u and %u result %d", inBody1.GetID().GetIndex(), inBody2.GetID().GetIndex(), (int)result);

	return result;
}

void D3E::JoltContactListener::OnContactAdded(const Body &inBody1, const Body &inBody2, const ContactManifold &inManifold, ContactSettings &ioSettings)
{
	// Expect bodies to be sorted
	if (!(inBody1.GetID() < inBody2.GetID()))
		JPH_BREAKPOINT;

	//Trace("Contact added %u (%08x) and %u (%08x)", inBody1.GetID().GetIndex(), inManifold.mSubShapeID1.GetValue(), inBody2.GetID().GetIndex(), inManifold.mSubShapeID2.GetValue());

	// Insert new manifold into state map
	{
		lock_guard lock(stateMutex_);
		SubShapeIDPair key(inBody1.GetID(), inManifold.mSubShapeID1, inBody2.GetID(), inManifold.mSubShapeID2);
		if (state_.find(key) != state_.end())
			JPH_BREAKPOINT; // Added contact that already existed
		state_[key] = StatePair(inManifold.mBaseOffset, inManifold.mRelativeContactPointsOn1);
	}

	if (inBody1.IsSensor() || inBody2.IsSensor())
	{
		std::cout << "Collided with sensor\n";
	}

	if (next_ != nullptr)
		next_->OnContactAdded(inBody1, inBody2, inManifold, ioSettings);
}

void D3E::JoltContactListener::OnContactPersisted(const Body &inBody1, const Body &inBody2, const ContactManifold &inManifold, ContactSettings &ioSettings)
{
	// Expect bodies to be sorted
	if (!(inBody1.GetID() < inBody2.GetID()))
		JPH_BREAKPOINT;

	//Trace("Contact persisted %u (%08x) and %u (%08x)", inBody1.GetID().GetIndex(), inManifold.mSubShapeID1.GetValue(), inBody2.GetID().GetIndex(), inManifold.mSubShapeID2.GetValue());

	// Update existing manifold in state map
	{
		lock_guard lock(stateMutex_);
		SubShapeIDPair key(inBody1.GetID(), inManifold.mSubShapeID1, inBody2.GetID(), inManifold.mSubShapeID2);
		StateMap::iterator i = state_.find(key);
		if (i != state_.end())
			i->second = StatePair(inManifold.mBaseOffset, inManifold.mRelativeContactPointsOn1);
		else
			JPH_BREAKPOINT; // Persisted contact that didn't exist
	}

	if (next_ != nullptr)
		next_->OnContactPersisted(inBody1, inBody2, inManifold, ioSettings);
}

void D3E::JoltContactListener::OnContactRemoved(const SubShapeIDPair &inSubShapePair)
{
	// Expect bodies to be sorted
	if (!(inSubShapePair.GetBody1ID() < inSubShapePair.GetBody2ID()))
		JPH_BREAKPOINT;

	//Trace("Contact removed %u (%08x) and %u (%08x)", inSubShapePair.GetBody1ID().GetIndex(), inSubShapePair.GetSubShapeID1().GetValue(), inSubShapePair.GetBody2ID().GetIndex(), inSubShapePair.GetSubShapeID2().GetValue());

	// Update existing manifold in state map
	{
		lock_guard lock(stateMutex_);
		StateMap::iterator i = state_.find(inSubShapePair);
		if (i != state_.end())
			state_.erase(i);
		else
			JPH_BREAKPOINT; // Removed contact that didn't exist
	}

	if (next_ != nullptr)
		next_->OnContactRemoved(inSubShapePair);
}

