#pragma once

#include "D3E/Components/render/LightComponent.h"
#include "D3E/systems/GameSystem.h"

namespace D3E
{
	class Game;

	class LightInitSystem : public GameSystem
	{
	public:
		explicit LightInitSystem(Game* game);

		static bool IsDirty;
		void PreDraw(entt::registry& reg, nvrhi::ICommandList* commandList, nvrhi::IDevice* device) override;

	private:
		Game* game_;

		void AddPointBSets(const String& uuid, const LightComponent& lc);
		void AddDirectionalBSets(const String& uuid, const LightComponent& lc);
	};
}
