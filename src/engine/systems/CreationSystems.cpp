#include "D3E/systems/CreationSystems.h"

#include "D3E/Components/FPSControllerComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Components/sound/SoundComponent.h"
#include "D3E/components/TransformComponent.h"
#include "D3E/components/render/StaticMeshComponent.h"
#include "engine/components/ObjectInfoComponent.h"

entt::entity D3E::CreationSystems::CreateCubeSM(entt::registry& registry)
{
	const auto e = registry.create();
	StaticMeshComponent sm;
	sm.meshName = "Cube";
	sm.pipelineName = "SimpleForward";

	ObjectInfoComponent info;
	info.name = "Cube";

	TransformComponent transform;
	transform.position_ = {0, 1, 0};
	transform.rotation_ = {0, 0, 0, 1};
	transform.scale_ = {1, 1, 1};

	SoundComponent sound;
	sound.fileName = "sfx.mp3";
	sound.is3D = true;
	sound.isLooping = true;
	sound.isStreaming = false;
	sound.location = transform.position_;

	registry.emplace<ObjectInfoComponent>(e, info);
	registry.emplace<TransformComponent>(e, transform);
	registry.emplace<StaticMeshComponent>(e, sm);
	registry.emplace<SoundComponent>(e, sound);

	return e;
}

entt::entity D3E::CreationSystems::CreateDefaultPlayer(entt::registry& registry)
{
	const auto e = registry.create();

	ObjectInfoComponent info;
	info.name = "Player";

	TransformComponent transform;
	transform.position_ = {0, 0, -10};
	transform.rotation_ = {0, 0, 0, 1};
	transform.scale_ = {1, 1, 1};

	CameraComponent camera;

	FPSControllerComponent fps;

	registry.emplace<ObjectInfoComponent>(e, info);
	registry.emplace<TransformComponent>(e, transform);
	registry.emplace<CameraComponent>(e, camera);
	registry.emplace<FPSControllerComponent>(e, fps);

	return e;
}
