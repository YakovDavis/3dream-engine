#pragma once

#include "EASTL/string.h"
#include "EASTL/vector.h"

namespace D3E
{
	class Game;

	struct ObjectInfo
	{
		eastl::string name;
	};

	class EditorUtils
	{
	public:
		static eastl::vector<ObjectInfo> ListActiveObjects();

	private:
		static void Initialize(Game* game);
		static Game* activeGame_;
		static bool initialized_;

	public:
		EditorUtils() = delete;

		friend class Game;
	};
}