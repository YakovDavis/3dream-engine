#include "D3E/systems/CreationSystems.h"

#include "D3E/CommonCpp.h"
#include "D3E/Components/FPSControllerComponent.h"
#include "D3E/Components/MouseComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Components/render/LightComponent.h"
#include "D3E/Components/sound/SoundComponent.h"
#include "D3E/Components/sound/SoundListenerComponent.h"
#include "D3E/Uuid.h"
#include "D3E/components/render/StaticMeshComponent.h"
#include "editor/EditorIdManager.h"
#include "render/components/GridComponent.h"
#include "render/systems/LightInitSystem.h"
#include "render/systems/StaticMeshInitSystem.h"

entt::entity D3E::CreationSystems::CreateCubeSM(entt::registry& registry,
                                                const ObjectInfoComponent& info,
                                                const TransformComponent& tc)
{
	const auto e = registry.create();
	StaticMeshComponent sm;
	sm.meshUuid = kCubeUUID;
	sm.pipelineName = "SimpleForward";

	ObjectInfoComponent infoComponent;
	infoComponent.name = info.name;
	infoComponent.id = UuidGenerator::NewGuidString();
	infoComponent.editorId = EditorIdManager::Get()->RegisterUuid(infoComponent.id);

	TransformComponent transform(tc);
	transform.position = tc.position;
	transform.rotation = tc.rotation;
	transform.scale = tc.scale;

	SoundComponent sound;
	sound.fileName = "sfx.mp3";
	sound.is3D = true;
	sound.isLooping = true;
	sound.isStreaming = false;
	sound.location = transform.position;

	registry.emplace<ObjectInfoComponent>(e, info);
	registry.emplace<TransformComponent>(e, tc);
	registry.emplace<StaticMeshComponent>(e, sm);
	StaticMeshInitSystem::IsDirty = true;
	registry.emplace<SoundComponent>(e, sound);

	return e;
}

entt::entity
D3E::CreationSystems::CreateDefaultPlayer(entt::registry& registry,
                                          const TransformComponent& tc)
{
	const auto e = registry.create();

	ObjectInfoComponent info;
	info.name = "Player";
	info.id = UuidGenerator::NewGuidString();

	TransformComponent transform(tc);

	CameraComponent camera;

	FPSControllerComponent fps;

	SoundListenerComponent slc;

	registry.emplace<ObjectInfoComponent>(e, info);
	registry.emplace<TransformComponent>(e, transform);
	registry.emplace<CameraComponent>(e, camera);
	registry.emplace<FPSControllerComponent>(e, fps);
	registry.emplace<SoundListenerComponent>(e, slc);

	return e;
}

entt::entity D3E::CreationSystems::CreateSM(
	entt::registry& registry, const D3E::ObjectInfoComponent& info,
	const D3E::TransformComponent& tc, const D3E::String& meshUuid, const D3E::String& materialUuid)
{
	const auto e = registry.create();
	StaticMeshComponent sm;
	sm.meshUuid = meshUuid;
	sm.pipelineName = "GBuffer";
	sm.materialUuid = materialUuid;

	ObjectInfoComponent infoComponent;
	infoComponent.name = info.name;
	infoComponent.id = UuidGenerator::NewGuidString();
	infoComponent.editorId = EditorIdManager::Get()->RegisterUuid(infoComponent.id);

	TransformComponent transform(tc);
	transform.position = tc.position;
	transform.rotation = tc.rotation;
	transform.scale = tc.scale;

	registry.emplace<ObjectInfoComponent>(e, infoComponent);
	registry.emplace<TransformComponent>(e, transform);
	registry.emplace<StaticMeshComponent>(e, sm);
	StaticMeshInitSystem::IsDirty = true;

	return e;
}

entt::entity D3E::CreationSystems::CreateLight(
	entt::registry& registry, const D3E::ObjectInfoComponent& info,
	const D3E::TransformComponent& tc)
{
	const auto e = registry.create();

	ObjectInfoComponent infoComponent;
	infoComponent.name = info.name;
	infoComponent.id = UuidGenerator::NewGuidString();
	infoComponent.editorId = EditorIdManager::Get()->RegisterUuid(infoComponent.id);

	TransformComponent transform(tc);
	transform.position = tc.position;
	transform.rotation = tc.rotation;
	transform.scale = tc.scale;

	LightComponent lc;

	registry.emplace<ObjectInfoComponent>(e, infoComponent);
	registry.emplace<TransformComponent>(e, transform);
	registry.emplace<LightComponent>(e, lc);
	LightInitSystem::IsDirty = true;

	return e;
}

entt::entity D3E::CreationSystems::CreateEditorDebugRender(entt::registry& registry)
{
	const auto e = registry.create();

	ObjectInfoComponent info;
	info.name = "EditorDebugRenderObject";
	info.id = UuidGenerator::NewGuidString();
	info.editorId = EditorIdManager::Get()->RegisterUuid(info.id);

	GridComponent grid;

	MouseComponent mouse;

	registry.emplace<ObjectInfoComponent>(e, info);
	registry.emplace<GridComponent>(e, grid);
	registry.emplace<MouseComponent>(e, mouse);

	return e;
}

entt::entity D3E::CreationSystems::CreatePhysicalCube(entt::registry& registry, const ObjectInfoComponent& info,
                                                             const TransformComponent& tc, const PhysicsComponent& physc)
{
	const auto e = registry.create();
	StaticMeshComponent sm;
	sm.meshUuid = kCubeUUID;
	sm.pipelineName = "GBuffer";

	ObjectInfoComponent infoComponent;
	infoComponent.name = info.name;
	infoComponent.id = UuidGenerator::NewGuidString();

	SoundComponent sound;
	sound.fileName = "sfx.mp3";
	sound.is3D = true;
	sound.isLooping = true;
	sound.isStreaming = false;
	sound.location = tc.position;

	registry.emplace<ObjectInfoComponent>(e, infoComponent);
	registry.emplace<TransformComponent>(e, tc);
	registry.emplace<StaticMeshComponent>(e, sm);
	registry.emplace<PhysicsComponent>(e, physc);
	StaticMeshInitSystem::IsDirty = true;
	registry.emplace<SoundComponent>(e, sound);

	return e;
}

entt::entity D3E::CreationSystems::CreatePhysicalCharacter(entt::registry& registry, const D3E::ObjectInfoComponent& info,
                                                           const D3E::TransformComponent& tc, const D3E::PhysicsCharacterComponent& character)
{
	const auto e = registry.create();

	ObjectInfoComponent infoComponent;
	infoComponent.name = info.name;
	infoComponent.id = UuidGenerator::NewGuidString();
	CameraComponent camera;
	camera.offset.z = 5 * character.colliderParams_.x;
	camera.offset.y = 2 * character.colliderParams_.x;
	camera.initialOffset = camera.offset;
	camera.forward = DirectX::SimpleMath::Vector3::Transform(camera.forward, tc.rotation);
	camera.forward.y = 0;
	camera.up = Vector3::Transform(Vector3::Up, DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(character.yaw_, character.pitch_, 0.0f));
	camera.offset = Vector3::Transform(camera.offset, DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(character.yaw_, 0.0f, 0.0f));

	registry.emplace<ObjectInfoComponent>(e, infoComponent);
	registry.emplace<CameraComponent>(e, camera);
	registry.emplace<TransformComponent>(e, tc);
	registry.emplace<PhysicsCharacterComponent>(e, character);

	return e;
}

entt::entity D3E::CreationSystems::CreatePurelyPhysicalObject(entt::registry& registry, const D3E::ObjectInfoComponent& info,
                                                              const D3E::TransformComponent& tc, const D3E::PhysicsComponent& physc)
{
	const auto e = registry.create();

	ObjectInfoComponent infoComponent;
	infoComponent.name = info.name;
	infoComponent.id = UuidGenerator::NewGuidString();

	registry.emplace<ObjectInfoComponent>(e, infoComponent);
	registry.emplace<TransformComponent>(e, tc);
	registry.emplace<PhysicsComponent>(e, physc);

	return e;
}