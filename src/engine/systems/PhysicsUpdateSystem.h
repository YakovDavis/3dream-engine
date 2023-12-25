#ifndef D3ENGINE_PHYSICSUPDATESYSTEM_H
#define D3ENGINE_PHYSICSUPDATESYSTEM_H

#include "D3E/systems/GameSystem.h"

#include <entt/entt.hpp>

namespace JPH
{
	class PhysicsSystem;
}

namespace D3E
{
	class PhysicsUpdateSystem : public GameSystem
	{
	public:
		explicit PhysicsUpdateSystem(JPH::PhysicsSystem* physicsSystem);
		void Update(entt::registry& reg, Game* game, float dT) override;

	private:
		JPH::PhysicsSystem* physicsSystem_;
	};
}

#endif // D3ENGINE_PHYSICSUPDATESYSTEM_H
