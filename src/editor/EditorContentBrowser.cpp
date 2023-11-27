#include "EditorContentBrowser.h"
#include "D3E/Debug.h"

const std::string AssetDirectory = "../../../";

D3E::EditorContentBrowser::EditorContentBrowser()
{
	open_ = true;
	auto path = std::filesystem::absolute(AssetDirectory).string();
	rootDirectory_ = path.substr(0, path.length() - 1);
	currentDirectory_ = rootDirectory_;
}

void D3E::EditorContentBrowser::Draw()
{
	ImGui::Begin("Content Browser");

	if(currentDirectory_ != rootDirectory_)
	{
		if(ImGui::Button("<-"))
		{
			currentDirectory_ = currentDirectory_.parent_path();
		}
	}

	for(auto& directoryEntry : std::filesystem::directory_iterator(currentDirectory_))
	{
		const auto& path = directoryEntry.path();
		auto relativePath = std::filesystem::relative(path, rootDirectory_);
		std::string fileNameString = relativePath.string();
		if(directoryEntry.is_directory())
		{
			if(ImGui::Button(fileNameString.c_str()))
			{
				currentDirectory_ /= path.filename();
			}
		}
		else
		{
			ImGui::Text(fileNameString.c_str());
//			if(ImGui::Button(fileNameString.c_str()))
//			{
//			}
		}
	}

	ImGui::End();
}

