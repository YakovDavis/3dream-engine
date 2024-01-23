#include "ScriptFactory.h"

#include "D3E/Debug.h"
#include "EASTL/unordered_map.h"
#include "utils/FilenameUtils.h"

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

void ScriptFactory::LoadScript(const ScriptMetaData& metaData,
                               const std::string& directory)
{
	using namespace std::filesystem;

	Debug::LogMessage("[ScriptFactory] Loading script file " +
	                  eastl::string(metaData.filename.c_str()));

	UnloadScript(metaData.uuid.c_str());

	auto scriptData = ScriptData();
	auto scriptFileName = path(metaData.filename).stem().string();

	scriptData.entryPoint = scriptFileName.c_str();

	auto path =
		FilenameUtils::MetaFilenameToFilePath(metaData.filename, directory);
	std::ifstream f(path);
	scriptData.scriptContent = std::string((std::istreambuf_iterator<char>(f)),
	                                       (std::istreambuf_iterator<char>()))
	                               .c_str();

	scriptsData_.insert({metaData.uuid.c_str(), scriptData});
}

void ScriptFactory::UnloadScript(const String& uuid)
{
	if (scriptsData_.find(uuid) == scriptsData_.end())
	{
		return;
	}

	scriptsData_.erase(uuid);
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

bool ScriptFactory::IsScriptUuidValid(const String& uuid)
{
	return scriptsData_.find(uuid) != scriptsData_.end();
}

void ScriptFactory::RenameScript(const D3E::String& uuid,
                                 const D3E::String& name)
{
	if (scriptsData_.find(uuid) == scriptsData_.end())
	{
		return;
	}

	scriptsData_[uuid].entryPoint = name;
}
