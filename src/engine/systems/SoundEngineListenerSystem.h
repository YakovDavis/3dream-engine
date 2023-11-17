#pragma once

#include "D3E/systems/PerTickSystem.h"

#include <entt/entt.hpp>

namespace D3E
{
	class SoundEngineListenerSystem : public PerTickSystem
	{
	public:
		SoundEngineListenerSystem(entt::registry& registry);
		void Run(entt::registry& reg, Game* game, float dT) override;

	private:
		entt::observer transformObserver_;
		entt::observer cameraObserver_;
	};
} // namespace D3E
