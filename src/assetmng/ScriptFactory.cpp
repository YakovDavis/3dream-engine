#include "ScriptFactory.h"

#include "D3E/Debug.h"
#include "EASTL/unordered_map.h"

#include <algorithm>
#include <filesystem>
#include <fstream>

using namespace D3E;

bool ScriptFactory::isInitialized_ = false;
Game* ScriptFactory::game_ = nullptr;
eastl::unordered_map<D3E::String, ScriptData> ScriptFactory::scriptsData_;

void ScriptFactory::Initialize(Game* game)
{
	if (isInitialized_)
	{
		return;
	}

	isInitialized_ = true;
	game_ = game;
}

void ScriptFactory::LoadScript(const ScriptMetaData& metaData)
{
	using namespace std::filesystem;

	Debug::LogMessage("[ScriptFactory] Loading script file " +
	                  eastl::string(metaData.filename.c_str()));

	auto scriptData = ScriptData();
	auto scriptFileName = path(metaData.filename).stem().string();

	/*std::transform(scriptFileName.begin(), scriptFileName.end(),
	               scriptFileName.begin(),
	               [](unsigned char c) { return std::tolower(c); });*/

	scriptData.entryPoint = scriptFileName.c_str();

	std::ifstream f(metaData.filename);
	scriptData.scriptContent = std::string((std::istreambuf_iterator<char>(f)),
	                                       (std::istreambuf_iterator<char>()))
	                               .c_str();

	scriptsData_.insert({metaData.uuid.c_str(), scriptData});
}

std::optional<ScriptData> ScriptFactory::GetScriptData(const String& uuid)
{
	if (scriptsData_.find(uuid) == scriptsData_.end())
	{
		Debug::LogError("[ScriptFactory] Script data not found. UUID: " + uuid);

		return std::nullopt;
	}

	return scriptsData_[uuid];
}
