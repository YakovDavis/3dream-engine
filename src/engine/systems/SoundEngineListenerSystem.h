#pragma once

#include "D3E/systems/GameSystem.h"

#include <entt/entt.hpp>

namespace D3E
{
	class SoundEngineListenerSystem : public GameSystem
	{
	public:
		explicit SoundEngineListenerSystem(entt::registry& registry);
		void Update(entt::registry& reg, Game* game, float dT) override;

	private:
		entt::observer transformObserver_;
		entt::observer cameraObserver_;
	};
} // namespace D3E
