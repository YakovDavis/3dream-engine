#pragma once

#include "D3E/systems/GameSystem.h"

#include <D3E/Components/navigation/NavmeshComponent.h>

namespace D3E
{
	class Game;
	class NavigationSystem : public GameSystem
	{
	public:
		NavigationSystem(Game* g);

		void Init() override;
		void Update(entt::registry& reg, Game* game, float dT) override;
		void Play(entt::registry& reg, Game* game) override;
		void Pause(entt::registry& reg, Game* game) override;
		void Stop(entt::registry& reg, Game* game) override;
		void StopReset(entt::registry& reg, Game* game) override;

	private:
		Game* game_;
		dtCrowdAgentDebugInfo crowdDebugInfo_;

		void InitCrowd(NavmeshComponent& nc);
		void InitAgents(entt::registry& reg, NavmeshComponent& nc);
	};
} // namespace D3E