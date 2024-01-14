#pragma once

#include "D3E/CommonHeader.h"
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
		Game* game_;

		ScriptingEngine();

		bool LoadDefaultEnvironment();
		void LoadStandardLibraries();

	public:
		static ScriptingEngine& GetInstance();

		ScriptingEngine(ScriptingEngine&) = delete;
		ScriptingEngine& operator=(ScriptingEngine const&) = delete;

		void Init(Game* g);
		bool LoadScript(ScriptComponent& c);
		void InitScriptComponent(ScriptComponent& c);
		void InitScripts();
		void StartScripts();
		void FreeScripts();
		void Clear();
	};
} // namespace D3E