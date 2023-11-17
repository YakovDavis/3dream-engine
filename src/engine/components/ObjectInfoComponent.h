#pragma once

#include "EASTL/string.h"

namespace D3E
{
	struct ObjectInfoComponent
	{
		ObjectInfoComponent* parent;
		eastl::string name;
	};
}
