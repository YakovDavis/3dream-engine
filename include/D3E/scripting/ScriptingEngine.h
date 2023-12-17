#pragma once

#include "D3E/Components/ScriptComponent.h"
#include "D3E/Uuid.h"
#include "sol/sol.hpp"

namespace D3E
{
	class Game;

	class ScriptingEngine
	{
	private:
		bool initialized_;
		sol::state luaState_;

		ScriptingEngine();

		bool LoadDefaultEnvironment();
		void LoadStandardLibraries();

	public:
		static ScriptingEngine& GetInstance();

		ScriptingEngine(ScriptingEngine&) = delete;
		~ScriptingEngine();
		ScriptingEngine& operator=(ScriptingEngine const&) = delete;

		void Init();
		bool InstantiateScriptComponent(ScriptComponent& c, String scriptUuid);
	};
} // namespace D3E