#pragma once

#include "D3E/CommonHeader.h"
#include "EASTL/fixed_vector.h"
#include "EASTL/string.h"
#include "json.hpp"

using json = nlohmann::json;

namespace D3E
{
	struct MeshMetaData
	{
		std::string uuid;
		std::string name;
		std::string filename;
	};

	void to_json(json& j, const MeshMetaData& t);

	void from_json(const json& j, MeshMetaData& t);
}
