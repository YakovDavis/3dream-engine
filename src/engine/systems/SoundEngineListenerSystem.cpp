#include "SoundEngineListenerSystem.h"

#include "D3E/Components/TransformComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Debug.h"
#include "D3E/Game.h"
#include "EASTL/fixed_vector.h"
#include "sound_engine/SoundEngine.h"

#include <format>

using namespace D3E;

SoundEngineListenerSystem::SoundEngineListenerSystem(entt::registry& registry)
	: transformObserver_{registry, entt::collector.update<TransformComponent>()
                                       .where<CameraComponent>()},
	  cameraObserver_{registry, entt::collector.update<CameraComponent>()}
{
}

void SoundEngineListenerSystem::Run(entt::registry& reg, Game* game, float dT)
{
	auto& se = SoundEngine::GetInstance();
	eastl::fixed_vector<float, 3, false> location{};
	eastl::fixed_vector<float, 3, false> up{};
	eastl::fixed_vector<float, 3, false> forward{};

	transformObserver_.each(
		[&](const auto entity)
		{
			const auto& transform = reg.get<TransformComponent>(entity);
			location = transform.position_;
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