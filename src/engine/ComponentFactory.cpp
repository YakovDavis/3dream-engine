#include "ComponentFactory.h"

#include "D3E/Components/FPSControllerComponent.h"
#include "D3E/Components/MouseComponent.h"
#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Components/PhysicsCharacterComponent.h"
#include "D3E/Components/PhysicsComponent.h"
#include "D3E/Components/ScriptComponent.h"
#include "D3E/Components/TransformComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Components/render/LightComponent.h"
#include "D3E/Components/render/SkyboxComponent.h"
#include "D3E/Components/render/StaticMeshComponent.h"
#include "D3E/Components/sound/SoundComponent.h"
#include "D3E/Components/sound/SoundListenerComponent.h"
#include "D3E/Game.h"
#include "core/EngineState.h"
#include "json.hpp"
#include "render/systems/LightInitSystem.h"
#include "render/systems/StaticMeshInitSystem.h"

bool D3E::ComponentFactory::isInitialized_ = false;
D3E::Game* D3E::ComponentFactory::game_;
eastl::map<entt::id_type, D3E::String> D3E::ComponentFactory::typeNames_;

void D3E::ComponentFactory::Initialize(D3E::Game* game)
{
	game_ = game;

	typeNames_.insert({entt::type_id<ObjectInfoComponent>().hash(), "ObjectInfoComponent"});
	typeNames_.insert({entt::type_id<TransformComponent>().hash(), "TransformComponent"});
	typeNames_.insert({entt::type_id<FPSControllerComponent>().hash(), "FPSControllerComponent"});
	typeNames_.insert({entt::type_id<MouseComponent>().hash(), "MouseComponent"});
	typeNames_.insert({entt::type_id<PhysicsComponent>().hash(), "PhysicsComponent"});
	typeNames_.insert({entt::type_id<PhysicsCharacterComponent>().hash(), "PhysicsCharacterComponent"});
	typeNames_.insert({entt::type_id<ScriptComponent>().hash(), "ScriptComponent"});
	typeNames_.insert({entt::type_id<CameraComponent>().hash(), "CameraComponent"});
	typeNames_.insert({entt::type_id<LightComponent>().hash(), "LightComponent"});
	typeNames_.insert({entt::type_id<SkyboxComponent>().hash(), "SkyboxComponent"});
	typeNames_.insert({entt::type_id<StaticMeshComponent>().hash(), "StaticMeshComponent"});
	typeNames_.insert({entt::type_id<SoundComponent>().hash(), "SoundComponent"});
	typeNames_.insert({entt::type_id<SoundListenerComponent>().hash(), "SoundListenerComponent"});
}

void D3E::ComponentFactory::DestroyResources()
{
}

entt::entity D3E::ComponentFactory::ResolveEntity(const json& j)
{
	auto e = game_->GetRegistry().create();

	for (auto el : j.at("components"))
	{
		if (el.at("class") == "ObjectInfoComponent")
		{
			ObjectInfoComponent c;
			c.from_json(el);
			game_->GetRegistry().emplace<ObjectInfoComponent>(e, c);
		}
		else if (el.at("class") == "TransformComponent")
		{
			TransformComponent c;
			c.from_json(el);
			game_->GetRegistry().emplace<TransformComponent>(e, c);
		}
		else if (el.at("class") == "FPSControllerComponent")
		{
			FPSControllerComponent c;
			c.from_json(el);
			game_->GetRegistry().emplace<FPSControllerComponent>(e, c);
		}
		else if (el.at("class") == "MouseComponent")
		{
			MouseComponent c;
			c.from_json(el);
			game_->GetRegistry().emplace<MouseComponent>(e, c);
		}
		else if (el.at("class") == "PhysicsComponent")
		{
			PhysicsComponent c;
			c.from_json(el);
			game_->GetRegistry().emplace<PhysicsComponent>(e, c);
		}
		else if (el.at("class") == "PhysicsCharacterComponent")
		{
			PhysicsCharacterComponent c;
			c.from_json(el);
			game_->GetRegistry().emplace<PhysicsCharacterComponent>(e, c);
		}
		else if (el.at("class") == "ScriptComponent")
		{
			ScriptComponent c(e);
			c.from_json(el);
			game_->GetRegistry().emplace<ScriptComponent>(e, c);
		}
		else if (el.at("class") == "CameraComponent")
		{
			CameraComponent c;
			c.from_json(el);
			game_->GetRegistry().emplace<CameraComponent>(e, c);
		}
		else if (el.at("class") == "LightComponent")
		{
			LightComponent c;
			c.from_json(el);
			game_->GetRegistry().emplace<LightComponent>(e, c);
			LightInitSystem::IsDirty = true;
		}
		else if (el.at("class") == "SkyboxComponent")
		{
			SkyboxComponent c;
			c.from_json(el);
			game_->GetRegistry().emplace<SkyboxComponent>(e, c);
		}
		else if (el.at("class") == "StaticMeshComponent")
		{
			StaticMeshComponent c;
			c.from_json(el);
			game_->GetRegistry().emplace<StaticMeshComponent>(e, c);
			StaticMeshInitSystem::IsDirty = true;
		}
		else if (el.at("class") == "SoundComponent")
		{
			SoundComponent c;
			c.from_json(el);
			game_->GetRegistry().emplace<SoundComponent>(e, c);
		}
		else if (el.at("class") == "SoundListenerComponent")
		{
			SoundListenerComponent c;
			c.from_json(el);
			game_->GetRegistry().emplace<SoundListenerComponent>(e, c);
		}
	}
	return e;
}

eastl::vector<D3E::String> D3E::ComponentFactory::GetAllEntityComponents(const entt::entity& e)
{
	eastl::vector<String> result;
	for(auto&& curr : game_->GetRegistry().storage())
	{
		auto& storage = curr.second;
		if(storage.contains(e))
		{
			entt::id_type id = curr.first;
			if (typeNames_.find(id) != typeNames_.end())
			{
				result.push_back(typeNames_[id]);
			}
		}
	}
	return result;
}

void D3E::ComponentFactory::SerializeEntity(const entt::entity& e, json& j,
                                            bool recordUuid)
{
	j = json({{"type", "entity"}, {"components", {}}});

	const auto list = GetAllEntityComponents(e);

	for (const auto& el : list)
	{
		if (el == "ObjectInfoComponent")
		{
			json c;
			game_->GetRegistry().get<ObjectInfoComponent>(e).to_json(c);
			if (!recordUuid)
			{
				c.at("uuid") = EmptyIdStdStr;
			}
			j.at("components").emplace_back(c);
		}
		else if (el == "TransformComponent")
		{
			json c;
			game_->GetRegistry().get<TransformComponent>(e).to_json(c);
			j.at("components").emplace_back(c);
		}
		else if (el == "FPSControllerComponent")
		{
			json c;
			game_->GetRegistry().get<FPSControllerComponent>(e).to_json(c);
			j.at("components").emplace_back(c);
		}
		else if (el == "MouseComponent")
		{
			json c;
			game_->GetRegistry().get<MouseComponent>(e).to_json(c);
			j.at("components").emplace_back(c);
		}
		else if (el == "PhysicsComponent")
		{
			json c;
			game_->GetRegistry().get<PhysicsComponent>(e).to_json(c);
			j.at("components").emplace_back(c);
		}
		else if (el == "PhysicsCharacterComponent")
		{
			json c;
			game_->GetRegistry().get<PhysicsCharacterComponent>(e).to_json(c);
			j.at("components").emplace_back(c);
		}
		else if (el == "ScriptComponent")
		{
			json c;
			game_->GetRegistry().get<ScriptComponent>(e).to_json(c);
			j.at("components").emplace_back(c);
		}
		else if (el == "CameraComponent")
		{
			json c;
			game_->GetRegistry().get<CameraComponent>(e).to_json(c);
			j.at("components").emplace_back(c);
		}
		else if (el == "LightComponent")
		{
			json c;
			game_->GetRegistry().get<LightComponent>(e).to_json(c);
			j.at("components").emplace_back(c);
		}
		else if (el == "SkyboxComponent")
		{
			json c;
			game_->GetRegistry().get<SkyboxComponent>(e).to_json(c);
			j.at("components").emplace_back(c);
		}
		else if (el == "StaticMeshComponent")
		{
			json c;
			game_->GetRegistry().get<StaticMeshComponent>(e).to_json(c);
			j.at("components").emplace_back(c);
		}
		else if (el == "SoundComponent")
		{
			json c;
			game_->GetRegistry().get<SoundComponent>(e).to_json(c);
			j.at("components").emplace_back(c);
		}
		else if (el == "SoundListenerComponent")
		{
			json c;
			game_->GetRegistry().get<SoundListenerComponent>(e).to_json(c);
			j.at("components").emplace_back(c);
		}
	}
}

void D3E::ComponentFactory::ResolveWorld(const json& j)
{
	for (const auto& el : j.at("entities"))
	{
		ResolveEntity(el);
	}
	EngineState::currentPlayer = game_->FindFirstNonEditorPlayer();
}

void D3E::ComponentFactory::SerializeWorld(json& j)
{
	std::string worldId = EmptyIdStdStr;

	if (j.contains("uuid"))
	{
		worldId = j.at("uuid");
	}

	j = json({{"type", "world"}, {"uuid", worldId}, {"entities", {}}});

	for (const auto& ent : game_->GetRegistry().storage<entt::entity>())
	{
		if (game_->GetRegistry().all_of<ObjectInfoComponent>(ent))
		{
			if (!game_->GetRegistry().get<ObjectInfoComponent>(ent).serializeEntity)
			{
				continue;
			}
		}
		else
		{
			continue;
		}
		json c;
		SerializeEntity(ent, c, true);
		j.at("entities").emplace_back(c);
	}
}
