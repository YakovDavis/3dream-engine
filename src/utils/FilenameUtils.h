#pragma once

#include "D3E/CommonHeader.h"
#include <filesystem>

namespace D3E
{
	class FilenameUtils
	{
	public:
		static std::string FilePathToMetaFilename(const std::string& filePath);
		static std::filesystem::path MetaFilenameToFilePath(const std::string& metaFilename, const std::filesystem::path& currentDir);
		static bool RenameAsset(std::filesystem::path& metaFilePath, const std::string& newName);
	};
}
