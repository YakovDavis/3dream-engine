#pragma once

#include "D3E/systems/PerRenderTickSystem.h"

namespace D3E
{
	class StaticMeshRenderSystem : public PerRenderTickSystem
	{
		void Render(entt::registry& reg, nvrhi::IFramebuffer*, nvrhi::CommandListHandle& commandList) override;
	};
}
