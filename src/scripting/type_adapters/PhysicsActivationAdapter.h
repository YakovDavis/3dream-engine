#ifndef D3ENGINE_PHYSICSACTIVATIONADAPTER_H
#define D3ENGINE_PHYSICSACTIVATIONADAPTER_H

#include <entt/entt.hpp>

namespace JPH
{
	class PhysicsSystem;
}

namespace D3E
{
	class PhysicsActivationAdapter
	{
	public:
		PhysicsActivationAdapter(entt::registry& registry, JPH::PhysicsSystem* physicsSystem);
		void SetActive(entt::entity entity, bool isActive);

	private:
		entt::registry& registry_;
		JPH::PhysicsSystem* physicsSystem_;
	};
}

#endif // D3ENGINE_PHYSICSACTIVATIONADAPTER_H
