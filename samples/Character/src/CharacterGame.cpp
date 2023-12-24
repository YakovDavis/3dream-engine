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

	character.colliderType_ = D3E::ColliderType::CapsuleCollider;
	character.colliderParams_ = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 0.0f);
	character.mass_ = 1.0f;
	character.friction_ = 0.2f;
	character.restitution_ = 0.0f;

	info.name = "Player";

	tc.position = Vector3(0.0f, 1.0f, 0.0f);

	D3E::CreationSystems::CreatePhysicalCharacter(GetRegistry(), info, tc, character);

	tc.position = Vector3(0, 0, 0);
	tc.scale = Vector3(50.0f, 1.0f, 50.0f);
	physComponent.colliderParams_ = DirectX::SimpleMath::Vector4(25.0f, 0.5f, 25.0f, 0.0f);
	info.name = "Floor";

	physComponent.colliderType_ = D3E::ColliderType::BoxCollider;
	physComponent.motionType_ = JPH::EMotionType::Static;

	D3E::CreationSystems::CreatePurelyPhysicalObject(GetRegistry(), info, tc, physComponent);

	info.name = "DirectionalLight";

	D3E::CreationSystems::CreateLight(GetRegistry(), info, tc);
}
