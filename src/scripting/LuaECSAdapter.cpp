#include "LuaECSAdapter.h"

#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Components/PhysicsCharacterComponent.h"
#include "D3E/Components/PhysicsComponent.h"
#include "D3E/Components/TransformComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Components/render/LightComponent.h"
#include "D3E/Components/render/StaticMeshComponent.h"
#include "D3E/Components/sound/SoundComponent.h"
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
			return sol::make_object(lua,
			                        registry_.try_get<ObjectInfoComponent>(e));
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
		default:
		{
			Debug::LogError(
				"[LuaECSAdapter] GetComponent(): unknown component type");

			return sol::nil;
		}
	}
}

sol::object LuaECSAdapter::GetScriptComponent(entt::entity e,
                                              const String& className,
                                              sol::this_state s)
{
	sol::state_view lua(s);

	auto sc = registry_.try_get<ScriptComponent>(e);

	if (!sc)
	{
		return sol::nil;
	}

	if (sc->GetEntryPoint() != className)
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

sol::object LuaECSAdapter::FindWithTag(entt::entity, const String tag,
                                       sol::this_state s)
{
	sol::state_view lua(s);
	auto entity = ECSUtils::GetEntityWithTag(registry_, tag);

	if (!entity)
	{
		return sol::nil;
	}

	return sol::make_object(lua, entity.value());
}
