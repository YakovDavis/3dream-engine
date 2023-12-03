#pragma once

#include "D3E/systems/GameSystem.h"

namespace D3E
{
	class LightInitSystem : public GameSystem
	{
	public:
		static bool IsDirty;
		void PreDraw(entt::registry& reg, nvrhi::ICommandList* commandList, nvrhi::IDevice* device) override;
	};
}
