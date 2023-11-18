#include "D3E/systems/CreationSystems.h"

#include "D3E/CommonCpp.h"
#include "D3E/Components/FPSControllerComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Components/sound/SoundComponent.h"
#include "D3E/Components/sound/SoundListenerComponent.h"
#include "D3E/components/render/StaticMeshComponent.h"

entt::entity D3E::CreationSystems::CreateCubeSM(entt::registry& registry,
                                                const ObjectInfoComponent& info,
                                                const TransformComponent& tc)
{
	const auto e = registry.create();
	StaticMeshComponent sm;
	sm.meshName = "Cube";
	sm.pipelineName = "SimpleForward";

	ObjectInfoComponent infoComponent;
	infoComponent.name = info.name;

	TransformComponent transform(tc);
	transform.position_ = tc.position_;
	transform.rotation_ = tc.rotation_;
	transform.scale_ = tc.scale_;

	SoundComponent sound;
	sound.fileName = "sfx.mp3";
	sound.is3D = true;
	sound.isLooping = true;
	sound.isStreaming = false;
	sound.location = transform.position_;

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
	//	transform.position_ = {0, 0, -10};
	//	transform.rotation_ = {0, 0, 0, 1};
	//	transform.scale_ = {1, 1, 1};

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
