#pragma once

#include "D3E/systems/GameSystem.h"

namespace D3E
{
	class EditorUtilsRenderSystem : public GameSystem
	{
	public:
		static bool isSelectionDirty;

		void InitRender(nvrhi::ICommandList* commandList, nvrhi::IDevice* device) override;
		void Update(entt::registry& reg, Game* game, float dT) override;
		void Draw(entt::registry& reg, nvrhi::IFramebuffer* fb, nvrhi::ICommandList* commandList, nvrhi::IDevice* device) override;
	};
}
