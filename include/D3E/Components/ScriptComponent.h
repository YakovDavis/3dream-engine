#pragma once

#include "D3E/CommonHeader.h"
#include "EASTL/string.h"
#include "entt/entt.hpp"
#include "sol/sol.hpp"

namespace D3E
{
	struct ScriptComponent
	{
		friend class ScriptingEngine;

		explicit ScriptComponent(entt::entity ownerId) : ownerId_{ownerId} {}

		void Init() { init(self); } // TODO(Denis): Add checks to all calls
		void Start() { start(self); }
		void Update(float deltaTime) { update(self, deltaTime); }

	private:
		entt::entity ownerId_;
		sol::object self;
		sol::function init;
		sol::function start;
		sol::function update;
	};
} // namespace D3E