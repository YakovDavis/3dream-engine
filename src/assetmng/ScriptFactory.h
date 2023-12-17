#pragma once

#include "D3E/CommonHeader.h"
#include "EASTL/unordered_map.h"
#include "ScriptData.h"
#include "ScriptMetaData.h"

#include <optional>

namespace D3E
{
	class Game;

	class ScriptFactory
	{
	public:
		ScriptFactory() = delete;

		static void Initialize(Game* game);
		static void LoadScript(const ScriptMetaData& metaData);
		static std::optional<ScriptData> GetScriptData(const String& uuid);

	private:
		static bool isInitialized_;
		static Game* game_;
		static eastl::unordered_map<String, ScriptData> scriptsData_;
	};
} // namespace D3E