#pragma once

#include "entt/entt.hpp"
#include "nvrhi/nvrhi.h"

namespace D3E
{
	class RenderSystem
	{
	public:
		virtual void Run(entt::registry& reg, nvrhi::IDevice* device, nvrhi::CommandListHandle& commandList) = 0;
	};
}
