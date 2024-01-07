#include "D3E/scripting/ScriptingEngine.h"

#include "D3E/Debug.h"
#include "D3E/Game.h"
#include "EngineTypeBindings.h"
#include "assetmng/ScriptFactory.h"
#include "scripting/LuaECSAdapter.h"

using namespace D3E;

ScriptingEngine::ScriptingEngine() : initialized_(false)
{
}

ScriptingEngine& ScriptingEngine::GetInstance()
{
	static ScriptingEngine instance;

	return instance;
}

ScriptingEngine::~ScriptingEngine()
{
}

void ScriptingEngine::Init(Game* g)
{
	Debug::LogMessage("[ScriptingEngine] Initializing...");

	LoadStandardLibraries();

	BindEngineTypes(luaState_);

	luaState_["Component"] = new LuaECSAdapter(g->GetRegistry());

	if (!LoadDefaultEnvironment())
		return;

	initialized_ = true;
}

bool ScriptingEngine::LoadScript(ScriptComponent& c, String scriptUuid)
{
	auto scriptData = ScriptFactory::GetScriptData(scriptUuid);

	if (!scriptData)
	{
		Debug::LogError("[ScriptingEngine] LoadScript(): "
		                "Script was not found. UUID: " +
		                scriptUuid);

		return false;
	}

	const auto& script = scriptData.value();

	luaState_.safe_script(
		script.scriptContent.c_str(),
		[&script, &scriptUuid](lua_State*, sol::protected_function_result pfr)
		{
			sol::error err = pfr;

			Debug::LogError("[ScriptingEngine] LoadScript(): "
		                    "Error accured while loading script: " +
		                    scriptUuid + " Entry point: " + script.entryPoint +
		                    " Error: " + err.what());

			return pfr;
		});

	c.entryPoint_ = script.entryPoint;

	InitScriptComponent(c);

	return true;
}

void ScriptingEngine::InitScriptComponent(ScriptComponent& c)
{
	auto userType = luaState_[c.entryPoint_.c_str()];
	// Calling Lua object Ctor
	c.self = userType["new"](
		userType,
		sol::nil); // TODO(Denis): Find a safer way to avoid Panic case

	c.start = userType["start"];
	c.start.set_error_handler(luaState_["error_handler"]);

	c.init = userType["init"];
	c.init.set_error_handler(luaState_["error_handler"]);

	c.update = userType["update"];
	c.update.set_error_handler(luaState_["error_handler"]);

	c.onCollisionEnter = userType["on_collision_enter"];
	c.onCollisionEnter.set_error_handler(luaState_["error_handler"]);

	c.onCollisionStay = userType["on_collision_stay"];
	c.onCollisionStay.set_error_handler(luaState_["error_handler"]);

	c.onCollisionExit = userType["on_collision_exit"];
	c.onCollisionExit.set_error_handler(luaState_["error_handler"]);

	c.self["owner_id"] = c.ownerId_;
}

bool ScriptingEngine::LoadDefaultEnvironment()
{
	auto scriptData = ScriptFactory::GetScriptData(kEnvironmentScriptId);

	if (!scriptData)
	{
		Debug::LogError("[ScriptingEngine] Init(): Default environment script "
		                "was not found in ScriptFactory.");

		return false;
	}

	luaState_.script(scriptData.value().scriptContent.c_str());

	return true;
}

void ScriptingEngine::LoadStandardLibraries()
{
	luaState_.open_libraries(sol::lib::base, sol::lib::string, sol::lib::debug,
	                         sol::lib::math, sol::lib::package, sol::lib::os,
	                         sol::lib::table);
}
