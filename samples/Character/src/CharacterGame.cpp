#include "CharacterGame.h"
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
	D3E::PhysicsCharacterComponent character = {};

	physComponent.colliderType_ = D3E::ColliderType::CapsuleCollider;
	physComponent.colliderParams_ = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 0.0f);
	physComponent.mass_ = 1.0f;
	physComponent.motionType_ = JPH::EMotionType::Dynamic;
	physComponent.friction_ = 0.2f;
	physComponent.restitution_ = 0.0f;

	info.name = "Player";

	tc.position = Vector3(0.0f, 1.0f, 0.0f);

	D3E::CreationSystems::CreatePhysicalCharacter(GetRegistry(), info, tc, physComponent, character);

	tc.position = Vector3(0, 0, 0);
	tc.scale = Vector3(50.0f, 1.0f, 50.0f);
	info.name = "Floor";

	physComponent.heightMapSize_ = 4;
	physComponent.heightMap_ = new float[physComponent.heightMapSize_];
	physComponent.heightMap_[0] = 20.0f;
	physComponent.heightMap_[1] = 0.0f;
	physComponent.heightMap_[2] = 0.0f;
	physComponent.heightMap_[3] = 20.0f;
	physComponent.colliderType_ = D3E::ColliderType::HeightFieldCollider;
	physComponent.motionType_ = JPH::EMotionType::Static;

	D3E::CreationSystems::CreatePurelyPhysicalObject(GetRegistry(), info, tc, physComponent);

	info.name = "DirectionalLight";

	D3E::CreationSystems::CreateLight(GetRegistry(), info, tc);
}
