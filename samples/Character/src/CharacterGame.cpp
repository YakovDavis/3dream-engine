#include "CharacterGame.h"

#include "D3E/Components/ScriptComponent.h"
#include "D3E/Debug.h"
#include "D3E/TimerHandle.h"
#include "D3E/TimerManager.h"
#include "D3E/engine/ConsoleManager.h"
#include "D3E/scripting/ScriptingEngine.h"
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
	character.colliderParams_ =
		DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 0.0f);
	character.mass_ = 1000.0f;
	character.friction_ = 0.2f;
	character.restitution_ = 0.0f;

	info.name = "Player";

	tc.position = Vector3(0.0f, 5.0f, 0.0f);

	auto e = D3E::CreationSystems::CreatePhysicalCharacter(GetRegistry(), info,
	                                                       tc, character);

	D3E::String scriptId = "72d4a952-074c-4223-8ded-40c09c182061";
	auto scriptComponent = D3E::ScriptComponent(e, scriptId);

	D3E::ScriptingEngine::GetInstance().LoadScript(scriptComponent, scriptId);

	GetRegistry().emplace<D3E::ScriptComponent>(e, scriptComponent);

	tc.scale = Vector3(50.0f, 1.0f, 50.0f);
	tc.position = Vector3(0.0f, 0.0f, 0.0f);
	//physComponent.colliderParams_ = DirectX::SimpleMath::Vector4(25.0f, 0.5f, 25.0f, 0.0f);
	physComponent.colliderType_ = D3E::ColliderType::HeightFieldCollider;
	physComponent.heightMapSize_ = 4;
	physComponent.heightMap_ = new float[physComponent.heightMapSize_ * physComponent.heightMapSize_];
	physComponent.heightMap_[0] = 2.0f;
	physComponent.heightMap_[1] = 1.5f;
	physComponent.heightMap_[2] = 1.0f;
	physComponent.heightMap_[3] = 1.5f;
	physComponent.heightMap_[4] = 0.0f;
	physComponent.heightMap_[5] = 0.0f;
	physComponent.heightMap_[6] = 0.0f;
	physComponent.heightMap_[7] = 0.0f;
	physComponent.heightMap_[8] = -2.0f;
	physComponent.heightMap_[9] = -1.5f;
	physComponent.heightMap_[10] = -1.0f;
	physComponent.heightMap_[11] = -0.5f;
	physComponent.heightMap_[12] = -4.0f;
	physComponent.heightMap_[13] = -3.0f;
	physComponent.heightMap_[14] = -2.0f;
	physComponent.heightMap_[15] = -1.0f;
	info.name = "Floor";

	//physComponent.colliderType_ = D3E::ColliderType::BoxCollider;
	physComponent.motionType_ = JPH::EMotionType::Static;

	D3E::CreationSystems::CreatePurelyPhysicalObject(GetRegistry(), info, tc,
	                                                 physComponent);

	tc.position = Vector3(0.0f, 0.0f, 20.0f);
	tc.scale = Vector3(3.0f, 3.0f, 3.0f);
	physComponent.colliderType_ = D3E::ColliderType::BoxCollider;
	physComponent.colliderParams_ =
		DirectX::SimpleMath::Vector4(1.5f, 1.5f, 1.5f, 0.0f);
	physComponent.isSensor_ = true;
	info.name = "Cube";
	physComponent.motionType_ = JPH::EMotionType::Static;
	D3E::CreationSystems::CreatePurelyPhysicalObject(GetRegistry(), info, tc,
	                                                 physComponent);

	info.name = "DirectionalLight";

	D3E::CreationSystems::CreateLight(GetRegistry(), info, tc);
}
