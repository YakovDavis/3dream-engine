#pragma once

#include "D3E/CommonHeader.h"
#include "entt/entt.hpp"

namespace D3E
{
	class Game;

	class EngineState
	{
	private:
		static Game* game_;

		static int viewportW, viewportH;

		static int gameViewportW, gameViewportH;

		static bool isInitialized;

		static void RefreshViewport();

	public:
		EngineState() = delete;

		static void Initialize(Game* game);

		static int GetViewportWidth();
		static int GetViewportHeight();

		static int GetGameViewportWidth();
		static int GetGameViewportHeight();

		static entt::entity currentPlayer;

		static bool isViewportDirty;

		static String CurrentMapUuid;
	};
}
