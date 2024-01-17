#ifndef D3ENGINE_PHYSICSINFO_H
#define D3ENGINE_PHYSICSINFO_H

#include "physics/ObjectLayers.h"

namespace JPH
{
	class PhysicsSystem;
	class TempAllocatorImpl;
	class JobSystemThreadPool;
}

namespace D3E
{
	class Game;
	class JoltDebugRenderer;
	class JoltContactListener;

	class PhysicsInfo
	{
	public:
		PhysicsInfo(Game* game);
		~PhysicsInfo();
		void updatePhysics();
		JPH::PhysicsSystem* getPhysicsSystem() { return physicsSystem_; }
		bool getIsPaused() { return isPaused_; }
		void setIsPaused(bool isPaused) { isPaused_ = isPaused; }

	private:
		JPH::PhysicsSystem* physicsSystem_;
		JPH::TempAllocatorImpl* tempAllocator_;
		JPH::JobSystemThreadPool* jobSystem_;
		BPLayerInterfaceImpl* bpLayerInterface_;
		ObjectVsBroadPhaseLayerFilterImpl* objectVsBroadPhaseLayerFilter_;
		ObjectLayerPairFilterImpl* objectLayerPairFilter_;
		JoltDebugRenderer* joltRenderer_;
		JoltContactListener* contactListener_;
		bool isPaused_;
		static constexpr int TEMP_ALLOCATOR_SIZE = 10 * 1024 * 1024;
		static constexpr int MAX_BODIES = 65536;
		static constexpr int NUM_BODY_MUTEXES = 0;
		static constexpr int MAX_BODY_PAIRS = 65536;
		static constexpr int MAX_CONSTRAINTS = 10240;
		static constexpr float DELTA_TIME = 1.0f / 60.0f;
		static constexpr int COLLISION_STEPS = 1;
	};
}

#endif // D3ENGINE_PHYSICSINFO_H
