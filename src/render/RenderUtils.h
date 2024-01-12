#pragma once

#include "D3E/CommonHeader.h"
#include "entt/entt.hpp"
#include "nvrhi/nvrhi.h"

namespace D3E
{
	class RenderUtils
	{
	public:
		RenderUtils() = delete;

		static void InvalidateWorldBuffers(entt::registry& reg);
	};
}
