#include "D3E/systems/CreationSystems.h"

#include "D3E/CommonCpp.h"
#include "D3E/Components/FPSControllerComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Components/sound/SoundComponent.h"
#include "D3E/Components/sound/SoundListenerComponent.h"
#include "D3E/components/render/StaticMeshComponent.h"
#include "D3E/Uuid.h"

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
	const D3E::TransformComponent& tc, D3E::String meshUuid)
{
	const auto e = registry.create();
	StaticMeshComponent sm;
	sm.meshUuid = meshUuid;
	sm.pipelineName = "SimpleForward";

	ObjectInfoComponent infoComponent;
	infoComponent.name = info.name;
	infoComponent.id = UuidGenerator::NewGuidString();

	TransformComponent transform(tc);
	transform.position = tc.position;
	transform.rotation = tc.rotation;
	transform.scale = tc.scale;

	registry.emplace<ObjectInfoComponent>(e, info);
	registry.emplace<TransformComponent>(e, tc);
	registry.emplace<StaticMeshComponent>(e, sm);

	return e;
}
