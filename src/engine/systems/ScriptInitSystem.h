#pragma once

#include "D3E/systems/GameSystem.h"
#include "entt/entt.hpp"

namespace D3E
{
	class ScriptInitSystem : public GameSystem
	{
	public:
		ScriptInitSystem(entt::registry& registry);
		~ScriptInitSystem();

		void ScriptCreated(entt::registry& registry, entt::entity e);
		void ScriptDestroyed(entt::registry& registry, entt::entity e);
	private:
		entt::registry& registry_;
	};
} // namespace D3E