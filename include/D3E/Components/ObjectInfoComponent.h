#pragma once

#include "D3E/CommonHeader.h"
#include "D3E/Uuid.h"
#include "json_fwd.hpp"

using json = nlohmann::json;

namespace D3E
{
	struct ObjectInfoComponent
	{
		String parentId = EmptyIdString;
		String name;
		String id;
		String tag;
		uint32_t editorId = 0;
		bool visible = true; // TODO: implement
	};

	void to_json(json& j, const ObjectInfoComponent& t);

	void from_json(const json& j, ObjectInfoComponent& t);
} // namespace D3E
