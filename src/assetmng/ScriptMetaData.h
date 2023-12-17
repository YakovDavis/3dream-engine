#pragma once

#include "D3E/CommonHeader.h"
#include "json.hpp"

using json = nlohmann::json;

namespace D3E
{
	struct ScriptMetaData
	{
		std::string uuid;
		std::string filename;
	};

	void to_json(json& j, const ScriptMetaData& t);
	void from_json(const json& j, ScriptMetaData& t);
} // namespace D3E