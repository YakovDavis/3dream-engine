#include "D3E/Components/ScriptComponent.h"

#include "D3E/CommonHeader.h"
#include "D3E/scripting/ScriptingEngine.h"
#include "core/magic_enum.hpp"
#include "json.hpp"

namespace D3E
{
	void to_json(json& j, const ScriptComponent& sc)
	{
		j = json{
			{"type", "component"},
			{"class", "ScriptComponent"},
			{"entryPoint", sc.GetEntryPoint().c_str()},
			{"ownerId", sc.GetOwnerId()},
		};
	}

	void from_json(const json& j, ScriptComponent& sc)
	{
		std::string entryPoint;
		entt::entity entity{};

		j.at("entryPoint").get_to(entryPoint);
		j.at("ownerId").get_to(entity);

		ScriptingEngine::GetInstance().InitScriptComponent(sc);
	}
} // namespace D3E