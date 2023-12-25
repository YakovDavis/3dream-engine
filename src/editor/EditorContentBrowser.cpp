#include "EditorContentBrowser.h"
#include "D3E/Debug.h"
#include "assetmng/TextureFactory.h"

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

	static float padding = 12.0f;
	static float thumbnailSize = 64.0f;
	float cellSize = thumbnailSize + padding;

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int) (panelWidth / cellSize);
	if(columnCount < 1)
	{
		columnCount = 1;
	}

	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	if (ImGui::BeginChild("Browser", ImVec2(0, -footer_height_to_reserve), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))
	{


		ImGui::Columns(columnCount, 0 , false);


		for (auto & directoryEntry : std::filesystem::directory_iterator(currentDirectory_))
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, rootDirectory_);
			std::string fileNameString = relativePath.filename().string();

			if (directoryEntry.is_directory())
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::ImageButton(TextureFactory::GetTextureHandle("2b7db204-d914-4d33-a4e4-dc7c7f9ff216"), {thumbnailSize, thumbnailSize}, {0, -1}, {-1, 0});
				if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					currentDirectory_ /= path.filename();
				}
				ImGui::PopStyleColor();

				ImGui::TextWrapped(fileNameString.c_str());
				ImGui::NextColumn();
			}
			else if (directoryEntry.path().extension().generic_string() == ".meta")
			{
				std::ifstream f(directoryEntry.path());
				json metadata = json::parse(f);

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				if (metadata.at("type") == "script")
				{
					ImGui::ImageButton(TextureFactory::GetTextureHandle("20bb535f-c03d-44d5-b287-95e091bbf976"), {thumbnailSize, thumbnailSize}, {0, -1}, {-1, 0});
					if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						Debug::LogMessage("Clicked on Script");
						// logic for clicking on script
					}

					ImGui::PopStyleColor();

					ImGui::TextWrapped(RemoveExtension(fileNameString).c_str());
					ImGui::NextColumn();
				}
				else
				{
					ImGui::ImageButton(TextureFactory::GetTextureHandle("e204189e-5bb5-4fe3-a3b9-92fb27ab4c96"), {thumbnailSize, thumbnailSize}, {0, -1}, {-1, 0});
					if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						Debug::LogMessage("Clicked on Asset");
						// logic for any other asset except script
					}

					ImGui::PopStyleColor();

					ImGui::TextWrapped(RemovePath(metadata.at("filename")).c_str());
					ImGui::NextColumn();
				}

			}
		}
	}
	ImGui::EndChild();
	ImGui::Separator();

	ImGui::Columns(1);
	ImGui::SliderFloat("Item Size", &thumbnailSize, 16, 512);

	ImGui::End();
}
std::string D3E::EditorContentBrowser::RemoveExtension(std::string str)
{
	size_t lastDot = str.find_last_of(".");
	if (lastDot == std::string::npos)
	{
		return str;
	}
	return str.substr(0, lastDot);
}
std::string D3E::EditorContentBrowser::RemovePath(std::string str)
{
	size_t lastSlash = str.find_last_of("/");
	if (lastSlash == std::string::npos)
	{
		return str;
	}
	return str.substr(lastSlash + 1, str.size());
}
