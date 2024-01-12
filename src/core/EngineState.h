#pragma once

#include "D3E/CommonHeader.h"

namespace D3E
{
	class Game;

	class EngineState
	{
	private:
		static Game* game_;

		static int viewportW, viewportH;

		static bool isInitialized;

		static void RefreshViewport();

	public:
		EngineState() = delete;

		static void Initialize(Game* game);

		static int GetViewportWidth();
		static int GetViewportHeight();

		static bool isViewportDirty;

		static String CurrentMapUuid;
	};
}
