#pragma once

#include "D3E/Components/ObjectInfoComponent.h"

#include <string>

namespace D3E
{
	struct InfoAdapter
	{
		std::string name;
		std::string tag;

		InfoAdapter(const ObjectInfoComponent& info)
		{
			name = info.name.c_str();
			tag = info.tag.c_str();
		}
	};
} // namespace D3E