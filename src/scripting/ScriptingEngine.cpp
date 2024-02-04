#include "D3E/scripting/ScriptingEngine.h"

#include "D3E/Debug.h"
#include "D3E/Game.h"
#include "EngineTypeBindings.h"
#include "RmlSolLua/RmlSolLua.h"
#include "assetmng/ScriptFactory.h"
#include "physics/PhysicsInfo.h"
#include "scripting/LuaECSAdapter.h"
#include "scripting/type_adapters/InputDeviceAdapter.h"
#include "scripting/type_adapters/PhysicsActivationAdapter.h"

using namespace D3E;

ScriptingEngine::ScriptingEngine() : game_(nullptr), initialized_(false)
{
}

ScriptingEngine& ScriptingEngine::GetInstance()
{
	static ScriptingEngine instance;

	return instance;
}

void ScriptingEngine::Init(Game* g)
{
	if (initialized_)
	{
		Debug::LogWarning("[ScriptingEngine] : Init(): ScriptingEngine has "
		                  "already been initialized.");

		return;
	}

	game_ = g;

	Debug::LogMessage("[ScriptingEngine] Initializing...");

	InitRmlLuaBindings();

	LoadStandardLibraries();

	BindEngineTypes(luaState_);

	InitGlobalObjects();

	if (!LoadDefaultEnvironment())
		return;

	initialized_ = true;
}

bool ScriptingEngine::LoadScript(ScriptComponent& c)
{
	if (!initialized_)
	{
		Debug::LogError(
			"[ScriptingEngine] : LoadScript(): Attempted to load script before "
			"ScriptingEngine initialization. Call "
			"ScriptingEngine::Init() first.");

		return false;
	}

	if (c.scriptUuid_ == EmptyIdString)
	{
		return false;
	}

	auto scriptData = ScriptFactory::GetScriptData(c.scriptUuid_);

	if (!scriptData)
	{
		Debug::LogError("[ScriptingEngine] : LoadScript(): "
		                "Script was not found. UUID: " +
		                c.scriptUuid_);

		return false;
	}

	const auto& script = scriptData.value();

	luaState_.safe_script(
		script.scriptContent.c_str(),
		[&script, &c](lua_State*, sol::protected_function_result pfr)
		{
			sol::error err = pfr;

			Debug::LogError("[ScriptingEngine] : LoadScript(): "
		                    "Error accured while loading script: " +
		                    c.scriptUuid_ + " Entry point: " +
		                    script.entryPoint + " Error: " + err.what());

			return pfr;
		});

	c.entryPoint_ = script.entryPoint;
	c.loaded_ = true;

	InitScriptComponent(c);

	return true;
}

void ScriptingEngine::InitScriptComponent(ScriptComponent& c)
{
	if (!initialized_)
	{
		Debug::LogError("[ScriptingEngine] : InitScriptComponent(): Attempted "
		                "to init script before "
		                "ScriptingEngine initialization. Call "
		                "ScriptingEngine::Init() first.");

		return;
	}

	auto userType = luaState_[c.entryPoint_.c_str()];
	// Calling Lua object Ctor
	c.self_ = userType["new"](
		userType,
		sol::nil); // TODO(Denis): Find a safer way to avoid Panic case

	c.start_ = userType["start"];
	c.start_.set_error_handler(luaState_["error_handler"]);

	c.init_ = userType["init"];
	c.init_.set_error_handler(luaState_["error_handler"]);

	c.update_ = userType["update"];
	c.update_.set_error_handler(luaState_["error_handler"]);

	c.drawGui_ = userType["draw_gui"];
	c.drawGui_.set_error_handler(luaState_["error_handler"]);

	c.onCollisionEnter_ = userType["on_collision_enter"];
	c.onCollisionEnter_.set_error_handler(luaState_["error_handler"]);

	c.onCollisionStay_ = userType["on_collision_stay"];
	c.onCollisionStay_.set_error_handler(luaState_["error_handler"]);

	c.onCollisionExit_ = userType["on_collision_exit"];
	c.onCollisionExit_.set_error_handler(luaState_["error_handler"]);

	c.onClicked_ = userType["on_clicked"];
	c.onClicked_.set_error_handler(luaState_["error_handler"]);

	sol::protected_function::set_default_handler(luaState_["error_handler"]);

	c.self_["owner_id"] = c.ownerId_;
}

bool ScriptingEngine::LoadDefaultEnvironment()
{
	auto scriptData = ScriptFactory::GetScriptData(kEnvironmentScriptId);

	if (!scriptData)
	{
		Debug::LogError(
			"[ScriptingEngine] : Init(): Default environment script "
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

void ScriptingEngine::InitGlobalObjects()
{
	luaState_["Component"] = new LuaECSAdapter(game_->GetRegistry());
	luaState_["Input"] = new InputDeviceAdapter(game_->GetInputDevice());
	luaState_["Physics"] = new PhysicsActivationAdapter(
		game_->GetRegistry(), game_->GetPhysicsInfo()->getPhysicsSystem());
}

void ScriptingEngine::InitRmlLuaBindings()
{
	Rml::SolLua::Initialise(&luaState_);
}

void ScriptingEngine::InitScripts()
{
	if (!initialized_)
	{
		Debug::LogError("[ScriptingEngine] : InitScripts(): Attempted "
		                "to init scripts before "
		                "ScriptingEngine initialization. Call "
		                "ScriptingEngine::Init() first.");

		return;
	}

	game_->GetRegistry().view<ScriptComponent>().each(
		[this](auto& sc)
		{
			LoadScript(sc);
			sc.Init();
		});
}

void ScriptingEngine::StartScripts()
{
	if (!initialized_)
	{
		Debug::LogError("[ScriptingEngine] : StartScripts(): Attempted "
		                "to start scripts before "
		                "ScriptingEngine initialization. Call "
		                "ScriptingEngine::Init() first.");

		return;
	}

	game_->GetRegistry().view<ScriptComponent>().each([](auto& sc)
	                                                  { sc.Start(); });
}

void ScriptingEngine::FreeScripts()
{
	if (!initialized_)
	{
		Debug::LogError("[ScriptingEngine] : FreeScripts(): Attempted "
		                "to free scripts before "
		                "ScriptingEngine initialization. Call "
		                "ScriptingEngine::Init() first.");

		return;
	}

	game_->GetRegistry().view<ScriptComponent>().each([](auto& sc)
	                                                  { sc.Free(); });
}

void ScriptingEngine::Clear()
{
	Debug::LogMessage("[ScriptingEngine] : Cleared");

	initialized_ = false;
	luaState_ = {};
}

void ScriptingEngine::SetErrorHandler(sol::function f)
{
	GetInstance().SetErrorHandlerInternal(f);
}

void ScriptingEngine::SetErrorHandlerInternal(sol::function f)
{
	f.set_error_handler(luaState_["error_handler"]);
}
