#include "ScriptUpdateSystem.h"

#include "D3E/Components/ScriptComponent.h"

using namespace D3E;

void ScriptUpdateSystem::Update(entt::registry& reg, Game* game, float dT)
{
	auto view = reg.view<ScriptComponent>();

	if (view.begin() == view.end())
	{
		return;
	}

	view.each([&dT](auto& sc) { 
		sc.Update(dT); 
		});
}
