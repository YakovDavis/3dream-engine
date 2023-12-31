#ifndef D3ENGINE_JOLTCONTACTLISTENER_H
#define D3ENGINE_JOLTCONTACTLISTENER_H

#include <Jolt/Jolt.h>
#include <Jolt/Math/Real.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/StateRecorder.h>
#include <Jolt/Core/Mutex.h>
#include <Jolt/Core/UnorderedMap.h>


using namespace JPH;

namespace D3E
{
	class JoltContactListener : public ContactListener
	{
	public:
		ValidateResult	OnContactValidate(const Body &inBody1, const Body &inBody2, RVec3Arg inBaseOffset, const CollideShapeResult &inCollisionResult) override;
		void OnContactAdded(const Body &inBody1, const Body &inBody2, const ContactManifold &inManifold, ContactSettings &ioSettings) override;
		void OnContactPersisted(const Body &inBody1, const Body &inBody2, const ContactManifold &inManifold, ContactSettings &ioSettings) override;
		void OnContactRemoved(const SubShapeIDPair &inSubShapePair) override;
		void SetNextListener(ContactListener *inListener) { next_ = inListener; }

	private:
		using StatePair = pair<RVec3, ContactPoints>;
		using StateMap = UnorderedMap<SubShapeIDPair, StatePair>;
		Mutex stateMutex_;
		StateMap state_;
		ContactListener* next_ = nullptr;

	};
}

#endif // D3ENGINE_JOLTCONTACTLISTENER_H
