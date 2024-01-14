#include "FilenameUtils.h"
#include <fstream>
#include "json.hpp"

std::string D3E::FilenameUtils::FilePathToMetaFilename(const std::string& filePath)
{
	return std::filesystem::path(filePath).filename().string();
}

std::filesystem::path D3E::FilenameUtils::MetaFilenameToFilePath(const std::string& metaFilename, const std::filesystem::path& currentDir)
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
	if (j.contains("filename"))
	{
		std::filesystem::path assetFile = MetaFilenameToFilePath(std::string(j.at("filename")), metaFilePath.parent_path());
		std::string assetExtension = assetFile.extension().string();
		rename(assetFile, assetFile.parent_path() / std::filesystem::path(newName + assetExtension));
		j.at("filename") = newName + assetExtension;
	}
	rename(metaFilePath, metaFilePath.parent_path() / std::filesystem::path(newName + ".meta"));
	metaFilePath.replace_filename(newName + ".meta");
	return true;
}
