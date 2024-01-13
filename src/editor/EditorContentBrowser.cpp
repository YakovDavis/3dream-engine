#include "EditorContentBrowser.h"

#include "D3E/Debug.h"
#include "assetmng/TextureFactory.h"
#include "engine/ComponentFactory.h"
#include "editor/Editor.h"
#include "D3E/Game.h"
#include "misc/cpp/imgui_stdlib.h"

#include <assetmng/ScriptMetaData.h>
#include <assetmng/MeshMetaData.h>
#include "D3E/render/Material.h"
#include <assetmng/SoundMetaData.h>
#include <cstdlib>
#include <iostream>

const std::string AssetDirectory = "assets/";
static std::string renamedItem = "";

D3E::EditorContentBrowser::EditorContentBrowser(Editor* editor)
{
	editor_ = editor;
	open_ = true;
	auto path = std::filesystem::absolute(AssetDirectory).string();
	rootDirectory_ = path.substr(0, path.length() - 1);
	currentDirectory_ = rootDirectory_;
}

void D3E::EditorContentBrowser::Draw()
{
	ImGui::Begin("Content Browser");

	ImGui::Button("New folder", ImVec2(0, 0));
	if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		std::filesystem::create_directory(currentDirectory_ / std::filesystem::path("new_folder"));
	}
	ImGui::SameLine();
	ImGui::Button("Import file", ImVec2(0, 0));
	if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		editor_->game_->AssetFileImport(currentDirectory_.string().c_str());
	}

	ImGui::SameLine();
	ImGui::Text(" Alt-LMB: delete, F2-LMB: rename folder");

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
			bool renamed = path == renamedItem;

			if (directoryEntry.is_directory())
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::ImageButton(TextureFactory::GetTextureHandle(
									   "2b7db204-d914-4d33-a4e4-dc7c7f9ff216"),
				                   {thumbnailSize, thumbnailSize}, {0, -1},
				                   {-1, 0});
				if (ImGui::IsItemHovered())
				{
					if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						if (ImGui::IsKeyDown(ImGuiKey_LeftAlt))
						{
							editor_->game_->AssetDeleteDialog(
								directoryEntry.path().string().c_str());
						}
						else if (ImGui::IsKeyDown(ImGuiKey_F2))
						{
							renamedItem = directoryEntry.path().string();
						}
					}
					if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						currentDirectory_ /= path.filename();
					}
				}
				ImGui::PopStyleColor();

				if (renamed)
				{
					ImGui::PushItemWidth(-1);
					ImGui::InputText("##input_label", &fileNameString);
					ImGui::PopItemWidth();
					if (ImGui::IsKeyDown(ImGuiKey_Enter))
					{
						std::filesystem::rename(
							directoryEntry.path(),
							directoryEntry.path().parent_path() /
								fileNameString);
						renamedItem = "";
					}
				}
				else
				{
					ImGui::TextWrapped(fileNameString.c_str());
				}
				ImGui::NextColumn();
			}
			else
			{
				if (directoryEntry.path().extension().generic_string() == ".meta")
				{
					std::ifstream f(directoryEntry.path());
					json metadata = json::parse(f);
					f.close();

					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
					if (metadata.at("type") == "script")
					{
						ScriptMetaData scriptMetadata;
						metadata.get_to(scriptMetadata);

						ImGui::ImageButton(
							TextureFactory::GetTextureHandle(
								"20bb535f-c03d-44d5-b287-95e091bbf976"),
							{thumbnailSize, thumbnailSize}, {0, -1}, {-1, 0});
						if (ImGui::IsItemHovered() &&
						    ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
							{
								if (ImGui::IsKeyDown(ImGuiKey_LeftAlt))
								{
									editor_->game_->AssetDeleteDialog(
										directoryEntry.path().string().c_str());
								}
								else if (ImGui::IsKeyDown(ImGuiKey_F2))
								{
									renamedItem =
										directoryEntry.path().string();
								}
							}
							if (ImGui::IsMouseDoubleClicked(
									ImGuiMouseButton_Left))
							{
								std::cout << std::flush;
								std::system(("code " + scriptMetadata.filename)
								                .c_str());
								tempUuid_ = scriptMetadata.uuid;
							}
						}

						ImGui::PopStyleColor();

						ImGui::TextWrapped(
							RemoveExtension(fileNameString).c_str());
						ImGui::NextColumn();
					}
					else if (metadata.at("type") == "world")
					{

						ImGui::ImageButton(
							TextureFactory::GetTextureHandle(
								"e204189e-5bb5-4fe3-a3b9-92fb27ab4c96"),
							{thumbnailSize, thumbnailSize}, {0, -1}, {-1, 0});
						if (ImGui::IsItemHovered() &&
						    ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
							{
								if (ImGui::IsKeyDown(ImGuiKey_LeftAlt))
								{
									editor_->game_->AssetDeleteDialog(
										directoryEntry.path().string().c_str());
								}
								else if (ImGui::IsKeyDown(ImGuiKey_F2))
								{
									renamedItem =
										directoryEntry.path().string();
								}
							}
							if (ImGui::IsMouseDoubleClicked(
									ImGuiMouseButton_Left))
							{
								ComponentFactory::ResolveWorld(metadata);
							}
						}

						ImGui::PopStyleColor();

						ImGui::TextWrapped(
							RemoveExtension(fileNameString).c_str());
						ImGui::NextColumn();
					}
					else if (metadata.at("type") == "model")
					{
						MeshMetaData meshMetaData;
						metadata.get_to(meshMetaData);

						ImGui::ImageButton(
							TextureFactory::GetTextureHandle(
								"e204189e-5bb5-4fe3-a3b9-92fb27ab4c96"),
							{thumbnailSize, thumbnailSize}, {0, -1}, {-1, 0});
						if (ImGui::IsItemHovered() &&
						    ImGui::IsMouseClicked(ImGuiMouseButton_Left))
						{
							if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
							{
								if (ImGui::IsKeyDown(ImGuiKey_LeftAlt))
								{
									editor_->game_->AssetDeleteDialog(
										directoryEntry.path().string().c_str());
								}
								else if (ImGui::IsKeyDown(ImGuiKey_F2))
								{
									renamedItem =
										directoryEntry.path().string();
								}
							}
							if (ImGui::IsMouseDoubleClicked(
									ImGuiMouseButton_Left))
							{
								tempUuid_ = meshMetaData.uuid.c_str();
							}
						}

						ImGui::PopStyleColor();

						ImGui::TextWrapped(
							RemovePath(metadata.at("name")).c_str());
						ImGui::NextColumn();
					}
					else if (metadata.at("type") == "material")
					{
						Material material;
						metadata.get_to(material);


						ImGui::ImageButton(
							TextureFactory::GetTextureHandle(
								"e204189e-5bb5-4fe3-a3b9-92fb27ab4c96"),
							{thumbnailSize, thumbnailSize}, {0, -1}, {-1, 0});
						if (ImGui::IsItemHovered() &&
						    ImGui::IsMouseClicked(ImGuiMouseButton_Left))
						{
							if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
							{
								if (ImGui::IsKeyDown(ImGuiKey_LeftAlt))
								{
									editor_->game_->AssetDeleteDialog(
										directoryEntry.path().string().c_str());
								}
								else if (ImGui::IsKeyDown(ImGuiKey_F2))
								{
									renamedItem =
										directoryEntry.path().string();
								}
							}
							if (ImGui::IsMouseDoubleClicked(
									ImGuiMouseButton_Left))
							{
								tempUuid_ = material.uuid.c_str();
							}
						}

						ImGui::PopStyleColor();

						ImGui::TextWrapped(
							RemovePath(metadata.at("name")).c_str());
						ImGui::NextColumn();
					}
					else if (metadata.at("type") == "sound")
					{
						SoundMetaData soundMetaData;
						metadata.get_to(soundMetaData);

						ImGui::ImageButton(
							TextureFactory::GetTextureHandle(
								"e204189e-5bb5-4fe3-a3b9-92fb27ab4c96"),
							{thumbnailSize, thumbnailSize}, {0, -1}, {-1, 0});
						if (ImGui::IsItemHovered() &&
						    ImGui::IsMouseClicked(ImGuiMouseButton_Left))
						{
							if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
							{
								if (ImGui::IsKeyDown(ImGuiKey_LeftAlt))
								{
									editor_->game_->AssetDeleteDialog(
										directoryEntry.path().string().c_str());
								}
								else if (ImGui::IsKeyDown(ImGuiKey_F2))
								{
									renamedItem =
										directoryEntry.path().string();
								}
							}
							if (ImGui::IsMouseDoubleClicked(
									ImGuiMouseButton_Left))
							{
								tempUuid_ = soundMetaData.uuid.c_str();
							}
						}

						ImGui::PopStyleColor();

						ImGui::TextWrapped(
							RemovePath(metadata.at("name")).c_str());
						ImGui::NextColumn();
					}
					else
					{

						ImGui::ImageButton(
							TextureFactory::GetTextureHandle(
								"e204189e-5bb5-4fe3-a3b9-92fb27ab4c96"),
							{thumbnailSize, thumbnailSize}, {0, -1}, {-1, 0});
						if (ImGui::IsItemHovered() &&
						    ImGui::IsMouseClicked(ImGuiMouseButton_Left))
						{
							if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
							{
								if (ImGui::IsKeyDown(ImGuiKey_LeftAlt))
								{
									editor_->game_->AssetDeleteDialog(
										directoryEntry.path().string().c_str());
								}
								else if (ImGui::IsKeyDown(ImGuiKey_F2))
								{
									renamedItem =
										directoryEntry.path().string();
								}
							}
							if (ImGui::IsMouseDoubleClicked(
									ImGuiMouseButton_Left))
							{
								Debug::LogMessage("Double-clicked on Asset");
								// logic for any other asset except script and world
							}
						}

						ImGui::PopStyleColor();

						ImGui::TextWrapped(
							RemovePath(metadata.at("name")).c_str());
						ImGui::NextColumn();
					}
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

std::string D3E::EditorContentBrowser::GetTempUuid()
{
	return tempUuid_;
}

void D3E::EditorContentBrowser::ResetTempUuid()
{
	tempUuid_ = "";
}
