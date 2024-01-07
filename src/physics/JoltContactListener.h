#ifndef D3ENGINE_JOLTCONTACTLISTENER_H
#define D3ENGINE_JOLTCONTACTLISTENER_H

#include <Jolt/Jolt.h>
#include <Jolt/Math/Real.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/StateRecorder.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Core/Mutex.h>
#include <Jolt/Core/UnorderedMap.h>
#include "D3E/Components/ScriptComponent.h"


using namespace JPH;

namespace D3E
{
	class Game;

	class JoltContactListener : public ContactListener
	{
	public:
		JoltContactListener(Game* g);

		ValidateResult	OnContactValidate(const Body &inBody1, const Body &inBody2, RVec3Arg inBaseOffset, const CollideShapeResult &inCollisionResult) override;
		void OnContactAdded(const Body &inBody1, const Body &inBody2, const ContactManifold &inManifold, ContactSettings &ioSettings) override;
		void OnContactPersisted(const Body &inBody1, const Body &inBody2, const ContactManifold &inManifold, ContactSettings &ioSettings) override;
		void OnContactRemoved(const SubShapeIDPair &inSubShapePair) override;
		void SetNextListener(ContactListener *inListener) { next_ = inListener; }

	private:
		using StatePair = pair<RVec3, ContactPoints>;
		using StateMap = UnorderedMap<SubShapeIDPair, StatePair>;
		Mutex stateMutex_;
		Mutex luaMutex_;
		StateMap state_;
		ContactListener* next_;
		Game* game_;

		void HandleBodyCollision(const BodyID& bodyId1, const BodyID& bodyId2,
		                         void (D3E::ScriptComponent::*handler)(const JPH::BodyID&));
		void HandleBodyCollision(const SubShapeIDPair& inSubShapePair,
			void (D3E::ScriptComponent::*handler)(const JPH::BodyID&));
		void RaiseCollisionEvent(const BodyID& firstBodyId,
		                         const BodyID& secondBodyId,
			void (D3E::ScriptComponent::*handler)(const JPH::BodyID&));
	};
}

#endif // D3ENGINE_JOLTCONTACTLISTENER_H
