#pragma once

#include "D3E/CommonHeader.h"
#include "D3E/Uuid.h"

namespace D3E
{
	struct ObjectInfoComponent
	{
		String parentId = EmptyIdString;
		String name;
		String id;
		uint32_t editorId = 0;
		bool visible = true; // TODO: implement
	};
} // namespace D3E
