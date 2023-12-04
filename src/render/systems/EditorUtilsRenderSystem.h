#pragma once

#include "D3E/systems/GameSystem.h"

namespace D3E
{
	class EditorUtilsRenderSystem : public GameSystem
	{
	public:
		void InitRender(nvrhi::ICommandList* commandList, nvrhi::IDevice* device) override;
		void Draw(entt::registry& reg, nvrhi::IFramebuffer* fb, nvrhi::ICommandList* commandList, nvrhi::IDevice* device) override;
	};
}
