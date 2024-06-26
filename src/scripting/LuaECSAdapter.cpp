#include "LuaECSAdapter.h"

#include "D3E/Components/AiAgentComponent.h"
#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Components/PhysicsCharacterComponent.h"
#include "D3E/Components/PhysicsComponent.h"
#include "D3E/Components/TransformComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Components/render/LightComponent.h"
#include "D3E/Components/render/StaticMeshComponent.h"
#include "D3E/Components/sound/SoundComponent.h"
#include "D3E/TimerManager.h"
#include "scripting/type_adapters/AiAgentAdapter.h"
#include "scripting/type_adapters/InfoAdapter.h"
#include "utils/ECSUtils.h"

using namespace D3E;

LuaECSAdapter::LuaECSAdapter(entt::registry& r) : registry_(r)
{
}

sol::object LuaECSAdapter::GetComponent(entt::entity e, ComponentType type,
                                        sol::this_state s)
{
	sol::state_view lua(s);

	switch (type)
	{
		case ComponentType::kTransformComponent:
		{
			return sol::make_object(lua,
			                        registry_.try_get<TransformComponent>(e));
		}
		case ComponentType::kPhysicsComponent:
		{
			return sol::make_object(lua,
			                        registry_.try_get<PhysicsComponent>(e));
		}
		case ComponentType::kPhysicsCharacterComponent:
		{
			return sol::make_object(
				lua, registry_.try_get<PhysicsCharacterComponent>(e));
		}
		case ComponentType::kObjectInfoComponent:
		{
			auto info = registry_.try_get<ObjectInfoComponent>(e);

			if (!info)
			{
				return sol::nil;
			}

			return sol::make_object(lua, InfoAdapter(*info));
		}
		case ComponentType::kSoundComponent:
		{
			return sol::make_object(lua, registry_.try_get<SoundComponent>(e));
		}
		case ComponentType::kLightComponent:
		{
			return sol::make_object(lua, registry_.try_get<LightComponent>(e));
		}
		case ComponentType::kCameraComponent:
		{
			return sol::make_object(lua, registry_.try_get<CameraComponent>(e));
		}
		case ComponentType::kStaticMeshComponent:
		{
			return sol::make_object(lua,
			                        registry_.try_get<StaticMeshComponent>(e));
		}
		case ComponentType::kAiAgentComponent:
		{
			auto aiAgent = registry_.try_get<AiAgentComponent>(e);

			if (!aiAgent)
			{
				return sol::nil;
			}

			return sol::make_object(lua, AiAgentAdapter(aiAgent));
		}
		default:
		{
			Debug::LogError(
				"[LuaECSAdapter] GetComponent(): unknown component type");

			return sol::nil;
		}
	}
}

sol::object LuaECSAdapter::GetScriptComponent(entt::entity e,
                                              const std::string& className,
                                              sol::this_state s)
{
	sol::state_view lua(s);

	auto sc = registry_.try_get<ScriptComponent>(e);

	if (!sc)
	{
		return sol::nil;
	}

	if (sc->GetEntryPoint() != className.c_str())
	{
		return sol::nil;
	}

	return sc->GetSelf();
}

sol::object LuaECSAdapter::FindWithBodyId(const JPH::BodyID& bodyId,
                                          sol::this_state s)
{
	sol::state_view lua(s);
	auto entity =
		ECSUtils::GetEntityByBodyId<PhysicsComponent>(registry_, bodyId);

	if (!entity)
	{
		return sol::nil;
	}

	return sol::make_object(lua, entity.value());
}

sol::object
D3E::LuaECSAdapter::FindWithCharacterBodyId(const JPH::BodyID& bodyId,
                                            sol::this_state s)
{
	sol::state_view lua(s);
	auto entity = ECSUtils::GetEntityByBodyId<PhysicsCharacterComponent>(
		registry_, bodyId);

	if (!entity)
	{
		return sol::nil;
	}

	return sol::make_object(lua, entity.value());
}

sol::object LuaECSAdapter::FindWithTag(const std::string& tag,
                                       sol::this_state s)
{
	sol::state_view lua(s);
	auto entity = ECSUtils::GetEntityWithTag(registry_, tag.c_str());

	if (!entity)
	{
		return sol::nil;
	}

	return sol::make_object(lua, entity.value());
}

eastl::vector<entt::entity>
D3E::LuaECSAdapter::FindAllWithTag(const std::string& tag)
{
	return ECSUtils::GetEntitiesWithTag(registry_, tag.c_str());
}

void D3E::LuaECSAdapter::Destroy(entt::entity e)
{
	ECSUtils::DestroyEntity(registry_, e);
}

void D3E::LuaECSAdapter::SelfDestroy(entt::entity e)
{
	TimerManager::GetInstance().SetTimerForNextTick(
		[this, e]() { ECSUtils::DestroyEntity(this->registry_, e); });
}

void D3E::LuaECSAdapter::DestroyMany(
	const eastl::vector<entt::entity>& entities)
{
	ECSUtils::DestroyEntities(registry_, entities);
}
