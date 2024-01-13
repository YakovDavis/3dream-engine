#ifndef D3ENGINE_CHARACTERINITSYSTEM_H
#define D3ENGINE_CHARACTERINITSYSTEM_H

#include "D3E/systems/GameSystem.h"

#include <entt/entt.hpp>

namespace JPH
{
	class PhysicsSystem;
}

namespace D3E
{

	class CharacterInitSystem : public GameSystem
	{
	public:
		CharacterInitSystem(entt::registry& registry, JPH::PhysicsSystem* physicsSystem);
		void Update(entt::registry& reg, Game* game, float dT) override;
		void PrePhysicsUpdate(entt::registry& reg, Game* game, float dT) override;
		void PostPhysicsUpdate(entt::registry& reg) override;

	private:
		void ComponentCreatedHandler(entt::registry& registry,
		                             entt::entity entity);
		entt::observer updateObserver_;
		JPH::PhysicsSystem* physicsSystem_;
	};
}

#endif // D3ENGINE_CHARACTERINITSYSTEM_H
