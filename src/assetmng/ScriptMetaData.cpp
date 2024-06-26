#include "ScriptMetaData.h"

#include "json.hpp"

namespace D3E
{
	using json = nlohmann::json;

	void to_json(json& j, const ScriptMetaData& t)
	{
		j = json{
			{"type", "script"}, {"uuid", t.uuid}, {"filename", t.filename}};
	}

	void from_json(const json& j, ScriptMetaData& t)
	{
		j.at("uuid").get_to(t.uuid);
		j.at("filename").get_to(t.filename);
	}
} // namespace D3E