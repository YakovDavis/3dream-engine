#include "ScriptInitSystem.h"

#include "D3E/Components/ScriptComponent.h"
#include "ScriptInitSystem.h"
#include "entt/entt.hpp"

using namespace D3E;

ScriptInitSystem::ScriptInitSystem(entt::registry& registry)
	: registry_{registry}
{
	registry_.on_construct<ScriptComponent>()
		.connect<&ScriptInitSystem::ScriptCreated>(this);
	registry_.on_destroy<ScriptComponent>()
		.connect<&ScriptInitSystem::ScriptDestroyed>(this);
}

ScriptInitSystem::~ScriptInitSystem()
{
	registry_.on_construct<ScriptComponent>()
		.disconnect<&ScriptInitSystem::ScriptCreated>(this);
	registry_.on_destroy<ScriptComponent>()
		.disconnect<&ScriptInitSystem::ScriptDestroyed>(this);
}

void ScriptInitSystem::ScriptCreated(entt::registry& registry, entt::entity e)
{
	registry.get<ScriptComponent>(e).Init();
}

void ScriptInitSystem::ScriptDestroyed(entt::registry& registry, entt::entity e)
{
	registry.get<ScriptComponent>(e).Free();
}