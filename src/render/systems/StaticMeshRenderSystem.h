#pragma once

#include "D3E/systems/GameSystem.h"

namespace D3E
{
	class StaticMeshRenderSystem : public GameSystem
	{
		void Draw(entt::registry& reg, nvrhi::IFramebuffer* fb, nvrhi::ICommandList* commandList, nvrhi::IDevice* device) override;
	};
}
