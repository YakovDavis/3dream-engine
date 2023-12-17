#include "D3E/scripting/ScriptingEngine.h"

#include "D3E/Debug.h"
#include "EngineTypeBindings.h"
#include "assetmng/ScriptFactory.h"

using namespace D3E;

ScriptingEngine::ScriptingEngine() : initialized_{false}
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

void ScriptingEngine::Init()
{
	Debug::LogMessage("[ScriptingEngine] Initializing...");

	LoadStandardLibraries();

	BindEngineTypes(luaState_);

	if (!LoadDefaultEnvironment())
		return;

	initialized_ = true;
}

bool ScriptingEngine::InstantiateScriptComponent(ScriptComponent& c,
                                                 String scriptUuid)
{
	auto scriptData = ScriptFactory::GetScriptData(scriptUuid);

	if (!scriptData)
	{
		Debug::LogError("[ScriptingEngine] InstantiateScriptComponent(): "
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

			Debug::LogError("[ScriptingEngine] InstantiateScriptComponent(): "
		                    "Error accured while loading script: " +
		                    scriptUuid + " Entry point: " + script.entryPoint +
		                    " Error: " + err.what());

			return pfr;
		});

	std::string entry = script.entryPoint.c_str();
	auto self = luaState_[entry];

	// Calling Lua object Ctor
	c.self = self["new"](
		self,
		sol::nil); // TODO(Denis): Find a safer way to avoid Panic case

	c.start = self["start"];
	c.init = self["init"];
	c.update = self["update"];

	return true;
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
