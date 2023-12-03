#pragma once

#include "D3E/systems/GameSystem.h"

namespace D3E
{
	class LightRenderSystem : public GameSystem
	{
	public:
		void Draw(entt::registry& reg, nvrhi::IFramebuffer* fb, nvrhi::ICommandList* commandList, nvrhi::IDevice* device) override;
	};
}
