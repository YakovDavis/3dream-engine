#ifndef D3ENGINE_PHYSICSINITSYSTEM_H
#define D3ENGINE_PHYSICSINITSYSTEM_H

#include "D3E/systems/GameSystem.h"

#include <entt/entt.hpp>

namespace JPH
{
	class PhysicsSystem;
}

namespace D3E
{
	class PhysicsInitSystem : public GameSystem
	{
	public:
		PhysicsInitSystem(entt::registry& registry, JPH::PhysicsSystem* physicsSystem);
		void Update(entt::registry& reg, Game* game, float dT) override;

	private:
		void ComponentCreatedHandler(entt::registry& registry,
		                             entt::entity entity);
		void ComponentDestroyedHandler(entt::registry& registry,
		                               entt::entity entity);
		entt::observer updateObserver_;
		JPH::PhysicsSystem* physicsSystem_;
	};
}

#endif // D3ENGINE_PHYSICSINITSYSTEM_H
