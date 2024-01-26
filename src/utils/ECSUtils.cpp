#include "utils/ECSUtils.h"

#include "D3E/TimerManager.h"
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

eastl::vector<entt::entity>
D3E::ECSUtils::GetEntitiesWithTag(entt::registry& r, const eastl::string& tag)
{
	eastl::vector<entt::entity> entities;

	auto view = r.view<ObjectInfoComponent>();

	for (auto [e, c] : view.each())
	{
		if (c.tag == tag)
		{
			entities.push_back(e);
		}
	}

	return entities;
}

D3E::ScriptComponent* D3E::ECSUtils::GetScriptComponent(entt::registry& r,
                                                        entt::entity e,
                                                        const String& className)
{
	auto sc = r.try_get<ScriptComponent>(e);

	if (sc->GetEntryPoint() == className)
	{
		return sc;
	}

	return nullptr;
}

void D3E::ECSUtils::DestroyEntity(entt::registry& r, entt::entity e)
{
	r.destroy(e);
}

void D3E::ECSUtils::DestroyEntities(entt::registry& r,
                                    const eastl::vector<entt::entity>& entities)
{
	r.destroy(entities.begin(), entities.end());
}

eastl::vector<entt::entity>
D3E::ECSUtils::GetEntitiesWithScript(entt::registry& r, const String& className)
{
	eastl::vector<entt::entity> entities;

	auto view = r.view<ScriptComponent>();

	for (auto [e, sc] : view.each())
	{
		if (sc.GetEntryPoint() == className)
		{
			entities.push_back(e);
		}
	}

	return entities;
}

void D3E::ECSUtils::LoadWorld(const std::string& path)
{
	TimerManager::GetInstance().SetTimerForNextTick( [path](){ LoadWorldInternal(path); });
}

void D3E::ECSUtils::LoadWorldInternal(const std::string& path)
{
	game_->LoadWorld(path);
}
