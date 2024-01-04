#pragma once

#include "D3E/CommonHeader.h"
#include "D3E/Components/ObjectInfoComponent.h"
#include "EASTL/vector.h"
#include "entt/entt.hpp"

namespace D3E
{
	class Game;

	struct EditorObjectInfo
	{
		ObjectInfoComponent* infoComponent;
		bool selected;
	};

	class EditorUtils
	{
	public:
		static eastl::vector<EditorObjectInfo> ListActiveObjects();

	private:
		static void Initialize(Game* game);
		static Game* activeGame_;
		static bool initialized_;

	public:
		EditorUtils() = delete;

		friend class Game;
	};
}