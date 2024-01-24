#pragma once

#include "D3E/CommonHeader.h"
#include "entt/entt.hpp"
#include "nvrhi/nvrhi.h"

namespace D3E
{
	class GameRender;
	class DebugRenderer;

	class RenderUtils
	{
	public:
		RenderUtils() = delete;

		static void Initialize(GameRender* gameRender);

		static DebugRenderer* GetDebugRenderer();

		static void InvalidateWorldBuffers(entt::registry& reg);

		static void GenerateMips(nvrhi::ITexture* texture, nvrhi::IDevice* device, nvrhi::ICommandList* commandList);

	private:
		static GameRender* gameRender_;
	};
}
