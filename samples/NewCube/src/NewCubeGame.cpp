#include "NewCubeGame.h"

#include "D3E/Debug.h"
#include "D3E/TimerHandle.h"
#include "D3E/TimerManager.h"
#include "D3E/engine/ConsoleManager.h"
#include "D3E/systems/CreationSystems.h"

#include <format>

void CharacterGame::Update(float deltaTime)
{
	Game::Update(deltaTime);
}

void CharacterGame::Init()
{
	Game::Init();

	D3E::TransformComponent tc = {};
	D3E::ObjectInfoComponent info = {};
	D3E::PhysicsComponent physComponent = {};

	physComponent.colliderType_ = D3E::ColliderType::BoxCollider;
	physComponent.colliderParams_ = DirectX::SimpleMath::Vector4(0.5f, 0.5f, 0.5f, 0.0f);
	physComponent.mass_ = 1.0f;
	physComponent.motionType_ = JPH::EMotionType::Dynamic;
	physComponent.velocity_ = DirectX::SimpleMath::Vector3(0.2f, 0.0f, 0.0f);
	physComponent.friction_ = 0.2f;
	physComponent.restitution_ = 1.0f;

	info.name = "Player";

	tc.position = Vector3(0, 0, 0);

	D3E::CreationSystems::CreateDefaultPlayer(GetRegistry(), tc);

	tc.position = Vector3(-5, 0, 0);
	tc.scale = Vector3(1.0f, 1.0f, 1.0f);
	info.name = "Cube1";

	D3E::CreationSystems::CreatePhysicalCube(GetRegistry(), info, tc, physComponent);

	physComponent.velocity_ = DirectX::SimpleMath::Vector3(-0.2f, 0.0f, 0.0f);
	tc.position = Vector3(5, 0, 0);
	info.name = "Cube2";

	D3E::CreationSystems::CreatePhysicalCube(GetRegistry(), info, tc, physComponent);

	info.name = "DirectionalLight";

	D3E::CreationSystems::CreateLight(GetRegistry(), info, tc);
}
