#pragma once

#include "D3E/CommonHeader.h"
#include "EASTL/vector.h"
#include "entt/entt.hpp"

namespace D3E
{
	class Game;

	struct ObjectInfo
	{
		String name;
		bool selected;
	};

	class EditorUtils
	{
	public:
		static eastl::vector<D3E::ObjectInfo> ListActiveObjects();

	private:
		static void Initialize(Game* game);
		static Game* activeGame_;
		static bool initialized_;

	public:
		EditorUtils() = delete;

		friend class Game;
	};
}