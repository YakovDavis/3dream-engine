#include "D3E/Components/ScriptComponent.h"

#include "D3E/CommonHeader.h"
#include "D3E/scripting/ScriptingEngine.h"
#include "core/magic_enum.hpp"
#include "json.hpp"

namespace D3E
{
	void to_json(json& j, const ScriptComponent& sc)
	{
		sc.to_json(j);
	}

	void from_json(const json& j, ScriptComponent& sc)
	{
		sc.from_json(j);
	}

	void ScriptComponent::to_json(json& j) const
	{
		j = json{
			{"type", "component"},
			{"class", "ScriptComponent"},
			{"entryPoint", GetEntryPoint().c_str()},
			{"ownerId", GetOwnerId()},
		};
	}

	void ScriptComponent::from_json(const json& j)
	{
		std::string entryPoint;
		entt::entity entity{};

		j.at("entryPoint").get_to(entryPoint);
		j.at("ownerId").get_to(entity);

		entryPoint_ = entryPoint.c_str();
		ownerId_ = entity;

		ScriptingEngine::GetInstance().InitScriptComponent(*this);
	}
} // namespace D3E