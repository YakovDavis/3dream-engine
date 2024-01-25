#include "D3E/systems/CreationSystems.h"

#include "D3E/CommonCpp.h"
#include "D3E/Components/FPSControllerComponent.h"
#include "D3E/Components/MouseComponent.h"
#include "D3E/Components/ScriptComponent.h"
#include "D3E/Components/TPSControllerComponent.h"
#include "D3E/Components/navigation/NavmeshComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Components/render/LightComponent.h"
#include "D3E/Components/render/SkyboxComponent.h"
#include "D3E/Components/sound/SoundComponent.h"
#include "D3E/Components/sound/SoundListenerComponent.h"
#include "D3E/Debug.h"
#include "D3E/Uuid.h"
#include "D3E/components/render/StaticMeshComponent.h"
#include "editor/EditorIdManager.h"
#include "render/components/GridComponent.h"
#include "render/systems/LightInitSystem.h"
#include "render/systems/StaticMeshInitSystem.h"

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
	const D3E::TransformComponent& tc, const D3E::String& meshUuid,
	const D3E::String& materialUuid)
{
	const auto e = registry.create();
	StaticMeshComponent sm;
	sm.meshUuid = meshUuid;
	sm.pipelineName = "GBuffer";
	sm.materialUuid = materialUuid;

	ObjectInfoComponent infoComponent = info;
	infoComponent.id = UuidGenerator::NewGuidString();
	infoComponent.editorId =
		EditorIdManager::Get()->RegisterUuid(infoComponent.id);

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

entt::entity
D3E::CreationSystems::CreateLight(entt::registry& registry,
                                  const D3E::ObjectInfoComponent& info,
                                  const D3E::TransformComponent& tc)
{
	const auto e = registry.create();

	ObjectInfoComponent infoComponent;
	infoComponent.name = info.name;
	infoComponent.id = UuidGenerator::NewGuidString();
	infoComponent.editorId =
		EditorIdManager::Get()->RegisterUuid(infoComponent.id);

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

entt::entity
D3E::CreationSystems::CreateEditorDebugRender(entt::registry& registry)
{
	const auto e = registry.create();

	ObjectInfoComponent info;
	info.name = "EditorDebugRenderObject";
	info.id = UuidGenerator::NewGuidString();
	info.serializeEntity = false;
	info.editorId = EditorIdManager::Get()->RegisterUuid(info.id);

	GridComponent grid;

	MouseComponent mouse;

	registry.emplace<ObjectInfoComponent>(e, info);
	registry.emplace<GridComponent>(e, grid);
	registry.emplace<MouseComponent>(e, mouse);

	return e;
}

entt::entity D3E::CreationSystems::CreatePhysicalCube(
	entt::registry& registry, const ObjectInfoComponent& info,
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
	// sound.fileName = "sfx.mp3";
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

entt::entity D3E::CreationSystems::CreatePhysicalCharacter(
	entt::registry& registry, const D3E::ObjectInfoComponent& info,
	const D3E::TransformComponent& tc,
	const D3E::PhysicsCharacterComponent& character)
{
	const auto e = registry.create();

	ObjectInfoComponent infoComponent;
	infoComponent.name = info.name;
	infoComponent.id = UuidGenerator::NewGuidString();
	CameraComponent camera;
	camera.offset.z = 5 * character.colliderParams_.x;
	camera.offset.y = 2 * character.colliderParams_.x;
	camera.initialOffset = camera.offset;
	camera.forward =
		DirectX::SimpleMath::Vector3::Transform(camera.forward, tc.rotation);
	camera.forward.y = 0;
	camera.up = Vector3::Transform(
		Vector3::Up, DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(
						 character.yaw_, character.pitch_, 0.0f));
	camera.offset = Vector3::Transform(
		camera.offset, DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(
						   character.yaw_, 0.0f, 0.0f));

	registry.emplace<ObjectInfoComponent>(e, infoComponent);
	registry.emplace<CameraComponent>(e, camera);
	registry.emplace<TransformComponent>(e, tc);
	registry.emplace<PhysicsCharacterComponent>(e, character);

	return e;
}

entt::entity D3E::CreationSystems::CreatePurelyPhysicalObject(
	entt::registry& registry, const D3E::ObjectInfoComponent& info,
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

entt::entity
D3E::CreationSystems::OnCreateObjectButtonPressed(entt::registry& registry,
                                                  int item)
{
	switch (item)
	{
		case 0:
			return CreateDefaultEmpty(registry);
		case 1:
			return CreateDefaultPlane(registry);
		case 2:
			return CreateDefaultCube(registry);
		case 3:
			return CreateDefaultSphere(registry);
		case 4:
			return CreateDefaultLight(registry);
		default:
			Debug::LogWarning("[CreationSystems] Unknown CreateButton item, "
			                  "creating Empty instead");
			return CreateDefaultEmpty(registry);
	}
}
entt::entity D3E::CreationSystems::CreateDefaultEmpty(entt::registry& registry)
{
	const auto e = registry.create();

	ObjectInfoComponent ic;
	ic.name = "NewEmpty";
	ic.id = UuidGenerator::NewGuidString();
	ic.editorId = EditorIdManager::Get()->RegisterUuid(ic.id);

	TransformComponent tc = {};

	registry.emplace<ObjectInfoComponent>(e, ic);
	registry.emplace<TransformComponent>(e, tc);

	return e;
}

entt::entity D3E::CreationSystems::CreateDefaultPlane(entt::registry& registry)
{
	const auto e = registry.create();

	ObjectInfoComponent ic;
	ic.name = "NewPlane";
	ic.id = UuidGenerator::NewGuidString();
	ic.editorId = EditorIdManager::Get()->RegisterUuid(ic.id);

	TransformComponent tc = {};

	StaticMeshComponent smc;
	smc.meshUuid = kPlaneUUID;
	smc.pipelineName = "GBuffer";
	smc.materialUuid = kDefaultGridMaterialUUID;

	registry.emplace<ObjectInfoComponent>(e, ic);
	registry.emplace<TransformComponent>(e, tc);
	registry.emplace<StaticMeshComponent>(e, smc);
	StaticMeshInitSystem::IsDirty = true;

	return e;
}

entt::entity D3E::CreationSystems::CreateDefaultCube(entt::registry& registry)
{
	const auto e = registry.create();

	ObjectInfoComponent ic;
	ic.name = "NewCube";
	ic.id = UuidGenerator::NewGuidString();
	ic.editorId = EditorIdManager::Get()->RegisterUuid(ic.id);

	TransformComponent tc = {};

	StaticMeshComponent smc;
	smc.meshUuid = kCubeUUID;
	smc.pipelineName = "GBuffer";
	smc.materialUuid = kDefaultGridMaterialUUID;

	registry.emplace<ObjectInfoComponent>(e, ic);
	registry.emplace<TransformComponent>(e, tc);
	registry.emplace<StaticMeshComponent>(e, smc);
	StaticMeshInitSystem::IsDirty = true;

	return e;
}

entt::entity D3E::CreationSystems::CreateDefaultSphere(entt::registry& registry)
{
	const auto e = registry.create();

	ObjectInfoComponent ic;
	ic.name = "NewSphere";
	ic.id = UuidGenerator::NewGuidString();
	ic.editorId = EditorIdManager::Get()->RegisterUuid(ic.id);

	TransformComponent tc = {};

	StaticMeshComponent smc;
	smc.meshUuid = kSphereUUID;
	smc.pipelineName = "GBuffer";
	smc.materialUuid = kDefaultGridMaterialUUID;

	registry.emplace<ObjectInfoComponent>(e, ic);
	registry.emplace<TransformComponent>(e, tc);
	registry.emplace<StaticMeshComponent>(e, smc);
	StaticMeshInitSystem::IsDirty = true;

	return e;
}

entt::entity D3E::CreationSystems::CreateDefaultLight(entt::registry& registry)
{
	const auto e = registry.create();

	ObjectInfoComponent ic;
	ic.name = "NewLight";
	ic.id = UuidGenerator::NewGuidString();
	ic.editorId = EditorIdManager::Get()->RegisterUuid(ic.id);

	TransformComponent tc = {};

	LightComponent lc = {};

	registry.emplace<ObjectInfoComponent>(e, ic);
	registry.emplace<TransformComponent>(e, tc);
	registry.emplace<LightComponent>(e, lc);
	LightInitSystem::IsDirty = true;

	return e;
}

void D3E::CreationSystems::CreateDefaultFPSControllerComponent(
	entt::registry& registry, entt::entity& entity)
{
	FPSControllerComponent component = {};
	registry.emplace<FPSControllerComponent>(entity, component);
}
void D3E::CreationSystems::CreateDefaultCameraComponent(
	entt::registry& registry, entt::entity& entity)
{
	CameraComponent component = {};
	registry.emplace<CameraComponent>(entity, component);
}

void D3E::CreationSystems::CreateDefaultLightComponent(entt::registry& registry,
                                                       entt::entity& entity)
{
	LightComponent component = {};
	registry.emplace<LightComponent>(entity, component);
	LightInitSystem::IsDirty = true;
}

void D3E::CreationSystems::CreateDefaultStaticMeshComponent(
	entt::registry& registry, entt::entity& entity)
{
	StaticMeshComponent component = {};
	registry.emplace<StaticMeshComponent>(entity, component);
	StaticMeshInitSystem::IsDirty = true;
}

void D3E::CreationSystems::CreateDefaultSoundComponent(entt::registry& registry,
                                                       entt::entity& entity)
{
	SoundComponent component = {};
	registry.emplace<SoundComponent>(entity, component);
}

void D3E::CreationSystems::CreateDefaultSoundListenerComponent(
	entt::registry& registry, entt::entity& entity)
{
	SoundListenerComponent component = {};
	registry.emplace<SoundListenerComponent>(entity, component);
}

void D3E::CreationSystems::CreateDefaultScriptComponent(
	entt::registry& registry, entt::entity& entity)
{
	ScriptComponent component(entity);
	registry.emplace<ScriptComponent>(entity, component);
}

entt::entity D3E::CreationSystems::CreateEditorFakePlayer(entt::registry& registry)
{
	const auto e = registry.create();

	ObjectInfoComponent info;
	info.name = "EditorPlayer";
	info.id = UuidGenerator::NewGuidString();
	info.internalObject = true;
	info.serializeEntity = false;

	TransformComponent transform = {};

	CameraComponent camera;

	FPSControllerComponent fps;

	registry.emplace<ObjectInfoComponent>(e, info);
	registry.emplace<TransformComponent>(e, transform);
	registry.emplace<CameraComponent>(e, camera);
	registry.emplace<FPSControllerComponent>(e, fps);

	return e;
}

entt::entity D3E::CreationSystems::CreateSkybox(entt::registry& registry)
{
	const auto e = registry.create();

	ObjectInfoComponent ic;
	ic.name = "Skybox";
	ic.id = UuidGenerator::NewGuidString();
	ic.serializeEntity = true;
	ic.internalObject = false;
	ic.editorId = 0;

	TransformComponent tc = {};

	SkyboxComponent sc = {};

	registry.emplace<ObjectInfoComponent>(e, ic);
	registry.emplace<TransformComponent>(e, tc);
	registry.emplace<SkyboxComponent>(e, sc);
	StaticMeshInitSystem::IsDirty = true;

	return e;
}

void D3E::CreationSystems::CreateDefaultNavigationComponent(
	entt::registry& registry, entt::entity& entity)
{
	NavmeshComponent nc = {};
	registry.emplace<NavmeshComponent>(entity, nc);
}

void D3E::CreationSystems::CreateDefaultTPSControllerComponent(
	entt::registry& registry, entt::entity& entity)
{
	TPSControllerComponent component = {};
	registry.emplace<TPSControllerComponent>(entity, component);
}
