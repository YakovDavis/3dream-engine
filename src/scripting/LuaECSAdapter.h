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
		explicit LuaECSAdapter(entt::registry& r);

		sol::object GetComponent(entt::entity e, ComponentType type,
		                         sol::this_state s);
		sol::object
		GetScriptComponent(entt::entity e,
		                   const std::string& className, sol::this_state s);
		sol::object FindWithBodyId(const JPH::BodyID& bodyId,
		                           sol::this_state s);
		sol::object FindWithCharacterBodyId(const JPH::BodyID& bodyId,
		                                    sol::this_state s);
		sol::object FindWithTag(const std::string& tag,
		                        sol::this_state s);
		eastl::vector<entt::entity> FindAllWithTag(const std::string& tag);
		void Destroy(entt::entity e);
		void SelfDestroy(entt::entity e);
		void DestroyMany(const eastl::vector<entt::entity>& entities);

	private:
		entt::registry& registry_;
	};
} // namespace D3E