#pragma once

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/BodyID.h"
#include "entt/entt.hpp"
#include "sol/sol.hpp"
#include "utils/ECSUtils.h"

namespace D3E
{
	class LuaECSAdapter
	{
	public:
		LuaECSAdapter(entt::registry& r);

		sol::object GetComponent(entt::entity e, ComponentType type,
		                         sol::this_state s);
		sol::object GetScriptComponent(entt::entity e, const String className,
		                               sol::this_state s);
		sol::object FindWithBodyId(const JPH::BodyID& bodyId,
		                           sol::this_state s);
		sol::object FindWithCharacterBodyId(const JPH::BodyID& bodyId, sol::this_state s);
		sol::object FindWithTag(entt::entity, const String tag,
		                        sol::this_state s);

	private:
		entt::registry& registry_;
	};
} // namespace D3E