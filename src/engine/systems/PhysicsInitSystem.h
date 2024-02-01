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
	class Game;

	class PhysicsInitSystem : public GameSystem
	{
	public:
		PhysicsInitSystem(entt::registry& registry, Game* game, JPH::PhysicsSystem* physicsSystem);
		void Update(entt::registry& reg, Game* game, float dT) override;
		void PrePhysicsUpdate(entt::registry& reg, Game* game, float dT) override;
		void PostPhysicsUpdate(entt::registry& reg) override;
		void Play(entt::registry& reg, Game* game) override;
		void Pause(entt::registry& reg, Game* game) override;

	private:
		void ComponentCreatedHandler(entt::registry& registry,
		                             entt::entity entity);
		void ComponentDestroyedHandler(entt::registry& registry,
		                               entt::entity entity);
		void OnCreateComponent(entt::registry& registry,
		                     entt::entity entity);
		void OnDestroyComponent(entt::registry& registry,
		                        entt::entity entity);
		entt::observer updateObserver_;
		Game* game_;
		JPH::PhysicsSystem* physicsSystem_;
		static constexpr float MARGIN = 1e-6;
	};
}

#endif // D3ENGINE_PHYSICSINITSYSTEM_H
