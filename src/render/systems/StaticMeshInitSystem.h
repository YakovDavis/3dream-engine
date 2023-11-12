#pragma once

#include "D3E/systems/RenderSystem.h"

namespace D3E
{
	class StaticMeshInitSystem : public RenderSystem
	{
		void Run(entt::registry& reg, nvrhi::IDevice* device, nvrhi::CommandListHandle& commandList) override;
	};
}
