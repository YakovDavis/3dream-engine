#include "SoundEngineListenerSystem.h"

#include "D3E/Components/TransformComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Components/sound/SoundListenerComponent.h"
#include "D3E/Debug.h"
#include "D3E/Game.h"
#include "sound_engine/SoundEngine.h"

#include <format>

using namespace D3E;

SoundEngineListenerSystem::SoundEngineListenerSystem(entt::registry& registry)
	: transformObserver_{registry, entt::collector.update<TransformComponent>()
                                       .where<SoundListenerComponent>()},
	  cameraObserver_{registry, entt::collector.update<CameraComponent>()
                                    .where<SoundListenerComponent>()}
{
}

void SoundEngineListenerSystem::Update(entt::registry& reg, Game* game, float dT)
{
	auto& se = SoundEngine::GetInstance();

	DirectX::SimpleMath::Vector3 location;
	DirectX::SimpleMath::Vector3 up;
	DirectX::SimpleMath::Vector3 forward;

	transformObserver_.each(
		[&](const auto entity)
		{
			const auto& transform = reg.get<TransformComponent>(entity);
			location = transform.position;
		});

	cameraObserver_.each(
		[&](const auto entity)
		{
			const auto& camera = reg.get<CameraComponent>(entity);
			up = camera.up;
			forward = camera.forward;
		});

	se.SetListenerTransform(location, {0, 0, 0}, forward, up);
}