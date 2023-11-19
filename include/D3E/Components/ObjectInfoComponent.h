#pragma once

#include "D3E/CommonHeader.h"
#include "D3E/Uuid.h"

namespace D3E
{
	struct ObjectInfoComponent
	{
		Uuid parentId = EmptyId;
		String name;
		Uuid id;
	};
} // namespace D3E
