#include "FilenameUtils.h"

#include "D3E/AssetManager.h"
#include "assetmng/MaterialFactory.h"
#include "assetmng/ScriptFactory.h"
#include "assetmng/TextureFactory.h"
#include "json.hpp"

#include <fstream>

std::string
D3E::FilenameUtils::FilePathToMetaFilename(const std::string& filePath)
{
	return std::filesystem::path(filePath).filename().string();
}

std::filesystem::path D3E::FilenameUtils::MetaFilenameToFilePath(
	const std::string& metaFilename, const std::filesystem::path& currentDir)
{
	return currentDir / std::filesystem::path(metaFilename);
}

bool D3E::FilenameUtils::RenameAsset(std::filesystem::path& metaFilePath,
                                     const std::string& newName)
{
	if (is_directory(metaFilePath))
	{
		return false;
	}

	if (!metaFilePath.has_extension())
	{
		return false;
	}

	if (metaFilePath.extension() != ".meta")
	{
		return false;
	}

	std::ifstream f(metaFilePath);
	json j = json::parse(f);
	f.close();

	if (j.contains("filename") && (j.at("type") != "world"))
	{
		std::filesystem::path assetFile = MetaFilenameToFilePath(
			std::string(j.at("filename")), metaFilePath.parent_path());
		std::string assetExtension = assetFile.extension().string();
		rename(assetFile, assetFile.parent_path() /
		                      std::filesystem::path(newName + assetExtension));
		j.at("filename") = newName + assetExtension;
	}

	if (j.contains("name"))
	{
		j.at("name") = newName;
	}

	if (j.contains("type") && j.contains("uuid"))
	{
		AssetManager::InsertOrReplaceAssetName(
			std::string(j.at("uuid")).c_str(), newName.c_str());

		if (j.at("type") == "texture2d")
		{
			TextureFactory::RenameTexture(std::string(j.at("uuid")).c_str(),
			                              newName.c_str());
		}
		if (j.at("type") == "material")
		{
			MaterialFactory::RenameMaterial(std::string(j.at("uuid")).c_str(),
			                                newName.c_str());
		}
		if (j.at("type") == "script")
		{
			ScriptFactory::RenameScript(std::string(j.at("uuid")).c_str(),
			                            newName.c_str());
		}
	}
	rename(metaFilePath, metaFilePath.parent_path() /
	                         std::filesystem::path(newName + ".meta"));
	metaFilePath.replace_filename(newName + ".meta");

	std::ofstream o(metaFilePath, std::ios::out | std::ios::trunc);
	o << std::setw(4) << j << std::endl;

	return true;
}
