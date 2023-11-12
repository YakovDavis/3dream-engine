#pragma once

#include "entt/entt.hpp"
#include "nvrhi/nvrhi.h"

namespace D3E
{
	class PerRenderTickSystem
	{
	public:
		virtual void Render(entt::registry& reg, nvrhi::IFramebuffer* fb, nvrhi::CommandListHandle& commandList) = 0;
	};
}
