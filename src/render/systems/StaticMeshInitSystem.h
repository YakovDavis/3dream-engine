#pragma once

#include "D3E/systems/GameSystem.h"

namespace D3E
{
	class StaticMeshInitSystem : public GameSystem
	{
		void PreDraw(entt::registry& reg, nvrhi::ICommandList* commandList, nvrhi::IDevice* device) override;
	};
}
