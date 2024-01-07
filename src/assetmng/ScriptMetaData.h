#pragma once

#include "D3E/CommonHeader.h"
#include "json.hpp"

namespace D3E
{
	struct ScriptMetaData
	{
		std::string uuid;
		std::string filename;
	};

	void to_json(nlohmann::json& j, const ScriptMetaData& t);
	void from_json(const nlohmann::json& j, ScriptMetaData& t);
} // namespace D3E