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
			{"script_uuid", scriptUuid_.c_str()},
			{"entryPoint", GetEntryPoint().c_str()},
		};
	}

	void ScriptComponent::from_json(const json& j)
	{
		std::string entryPoint;
		std::string scriptUuid;

		j.at("entryPoint").get_to(entryPoint);
		j.at("script_uuid").get_to(scriptUuid);

		entryPoint_ = entryPoint.c_str();
		scriptUuid_ = scriptUuid.c_str();

		ScriptingEngine::GetInstance().InitScriptComponent(*this);
	}
} // namespace D3E