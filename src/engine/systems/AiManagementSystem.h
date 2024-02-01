#pragma once

#include "D3E/systems/GameSystem.h"

namespace D3E
{
	class AiManagementSystem : public GameSystem
	{
	public:
		AiManagementSystem();

		void Update(entt::registry& reg, Game* game, float dT) override;

		void Play(entt::registry& reg, Game* game) override;

		void Pause(entt::registry& reg, Game* game) override;

		void Stop(entt::registry& reg, Game* game) override;

	private:
	};
} // namespace D3E