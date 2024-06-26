#pragma once

#include "D3E/CommonHeader.h"
#include "D3E/Components/ScriptComponent.h"
#include "D3E/Uuid.h"
#include "sol/sol.hpp"

namespace D3E
{
	class Game;

	class D3EAPI ScriptingEngine
	{
	private:
		bool initialized_;
		sol::state luaState_;
		Game* game_;

		ScriptingEngine();

		bool LoadDefaultEnvironment();
		void LoadStandardLibraries();
		void InitGlobalObjects();
		void SetErrorHandlerInternal(sol::function f);
	public:
		static ScriptingEngine& GetInstance();

		ScriptingEngine(ScriptingEngine&) = delete;
		ScriptingEngine& operator=(ScriptingEngine const&) = delete;

		void Init(Game* g);
		bool LoadScript(ScriptComponent& c);
		void InitScriptComponent(ScriptComponent& c);
		void InitRmlLuaBindings();
		void InitScripts();
		void StartScripts();
		void FreeScripts();
		void Clear();
		static void SetErrorHandler(sol::function f);
	};
} // namespace D3E