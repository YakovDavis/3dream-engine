#include "CubeGame.h"

#include "D3E/systems/CreationSystems.h"
#include "D3E/engine/ConsoleManager.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/MotionType.h>

void CubeGame::Update(float deltaTime)
{
	Game::Update(deltaTime);
}

void CubeGame::Init()
{
	Game::Init();

	D3E::TransformComponent tc = {};
	D3E::ObjectInfoComponent info = {};
    D3E::PhysicsComponent physComponent = {};

    physComponent.colliderType_ = D3E::ColliderType::BoxCollider;
    physComponent.colliderParams_ = DirectX::SimpleMath::Vector4(0.5f, 0.5f, 0.5f, 0.0f);
    physComponent.mass_ = 1.0f;
    physComponent.motionType_ = JPH::EMotionType::Dynamic;
    physComponent.velocity_ = DirectX::SimpleMath::Vector3(0.1f, 0.0f, 0.0f);

    D3E::PhysicsComponent physComponent2 = physComponent;
    physComponent2.velocity_ = DirectX::SimpleMath::Vector3(-0.1f, 0.0f, 0.0f);

	info.name = "Cube";

	tc.position = Vector3(0, 0, 10);

	D3E::CreationSystems::CreateDefaultPlayer(GetRegistry(), tc);

	tc.position = Vector3(-5, 0, 0);

	D3E::CreationSystems::CreatePhysicalCube(GetRegistry(), info, tc, physComponent);

    tc.position = Vector3(5, 0, 0);

    D3E::CreationSystems::CreatePhysicalCube(GetRegistry(), info, tc, physComponent2);
}
