#include "utils/ECSUtils.h"

#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Components/PhysicsComponent.h"
#include "D3E/Game.h"

D3E::Game* D3E::ECSUtils::game_ = nullptr;

void D3E::ECSUtils::Init(Game* g)
{
	D3E::ECSUtils::game_ = g;
}

std::optional<entt::entity>
D3E::ECSUtils::GetEntityByBodyId(entt::registry& r, const JPH::BodyID& bodyId)
{
	auto view = r.view<PhysicsComponent>();

	for (auto [e, pc] : view.each())
	{
		if (pc.bodyID_ == bodyId)
		{
			return e;
		}
	}

	return std::nullopt;
}

std::optional<entt::entity>
D3E::ECSUtils::GetEntityWithTag(entt::registry& r, const eastl::string& tag)
{
	auto view = r.view<ObjectInfoComponent>();

	for (auto [e, c] : view.each())
	{
		if (c.tag == tag)
		{
			return e;
		}
	}

	return std::nullopt;
}

D3E::ScriptComponent* D3E::ECSUtils::GetScriptComponent(entt::registry& r,
                                                        entt::entity e,
                                                        const String className)
{
	auto view = r.view<ScriptComponent>();

	for (auto [e, c] : view.each())
	{
	}

	return nullptr;
}
