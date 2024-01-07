#include "JoltDebugRenderer.h"

#include "PhysicsInfo.h"
#include "JoltContactListener.h"
#include "D3E/Game.h"

#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

using namespace JPH;

D3E::PhysicsInfo::PhysicsInfo(Game* game)
{
	RegisterDefaultAllocator();
	Factory::sInstance = new Factory();
	RegisterTypes();
	tempAllocator_ = new TempAllocatorImpl (TEMP_ALLOCATOR_SIZE);
	jobSystem_ = new JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);
	bpLayerInterface_ = new BPLayerInterfaceImpl;
	objectVsBroadPhaseLayerFilter_ = new ObjectVsBroadPhaseLayerFilterImpl;
	objectLayerPairFilter_ = new ObjectLayerPairFilterImpl;
	physicsSystem_ = new PhysicsSystem;
	physicsSystem_->Init(MAX_BODIES, NUM_BODY_MUTEXES, MAX_BODY_PAIRS, MAX_CONSTRAINTS, *bpLayerInterface_, *objectVsBroadPhaseLayerFilter_, *objectLayerPairFilter_);
	joltRenderer_ = new JoltDebugRenderer(game);
	contactListener_ = new JoltContactListener(game);
	//contactListener_->SetNextListener(mTest->GetContactListener());
	physicsSystem_->SetContactListener(contactListener_);
}

D3E::PhysicsInfo::~PhysicsInfo()
{
	UnregisterTypes();
	delete Factory::sInstance;
	Factory::sInstance = nullptr;
	delete bpLayerInterface_;
	delete objectVsBroadPhaseLayerFilter_;
	delete objectLayerPairFilter_;
	delete physicsSystem_;
	delete joltRenderer_;
	delete contactListener_;
}

void D3E::PhysicsInfo::updatePhysics()
{
	physicsSystem_->Update(DELTA_TIME, COLLISION_STEPS, tempAllocator_, jobSystem_);
	BodyManager::DrawSettings settings;
	physicsSystem_->DrawBodies(settings, joltRenderer_);

}
