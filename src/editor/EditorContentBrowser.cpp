#include "EditorContentBrowser.h"

#include "D3E/AssetManager.h"
#include "D3E/Debug.h"
#include "D3E/Game.h"
#include "D3E/render/Material.h"
#include "assetmng/TextureFactory.h"
#include "editor/Editor.h"
#include "engine/ComponentFactory.h"
#include "misc/cpp/imgui_stdlib.h"
#include "utils/FilenameUtils.h"

#include <assetmng/MeshMetaData.h>
#include <assetmng/ScriptMetaData.h>
#include <assetmng/SoundMetaData.h>
#include <cstdlib>
#include <iostream>

const std::string AssetDirectory = "assets/";
static std::string renamedItem = "";

#define CONTENT_BROWSER_COMMON_ACTIONS \
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) \
	{ \
		if (ImGui::IsKeyDown(ImGuiKey_LeftAlt)) \
		{ \
			editor_->game_->AssetDeleteDialog( \
				directoryEntry.path().string().c_str()); \
		} \
		else if (ImGui::IsKeyDown(ImGuiKey_F2)) \
		{ \
			renamedItem = \
				directoryEntry.path().string(); \
		} \
	}

#define ASSET_NAME_DISPLAY \
	if (renamed) \
	{ \
		ImGui::PushItemWidth(-1); \
		ImGui::InputText("##input_label", &fileNameStringNoExtension); \
		ImGui::PopItemWidth(); \
		if (ImGui::IsKeyDown(ImGuiKey_Enter)) \
		{ \
			std::filesystem::path newPath =  directoryEntry.path(); \
			FilenameUtils::RenameAsset(newPath, fileNameStringNoExtension); \
			renamedItem = ""; \
		} \
	} \
	else \
	{ \
		ImGui::TextWrapped(fileNameStringNoExtension.c_str()); \
	}

#define ASSET_DRAG_N_DROP_SOURCE \
	if (ImGui::BeginDragDropSource(dragDropFlags)) \
		{ \
			std::string path = directoryEntry.path().string(); \
			ImGui::SetDragDropPayload("asset", &path[0], path.size() + sizeof(std::string::value_type)); \
			ImGui::EndDragDropSource(); \
		}

D3E::EditorContentBrowser::EditorContentBrowser(Editor* editor)
{
	editor_ = editor;
	open_ = true;
	auto path = std::filesystem::absolute(AssetDirectory).string();
	rootDirectory_ = path.substr(0, path.length() - 1);
	currentDirectory_ = rootDirectory_;
	editor_->game_->SetContentBrowserFilePath(currentDirectory_.string());
}

void D3E::EditorContentBrowser::Draw()
{
	ImGuiDragDropFlags dragDropFlags = ImGuiDragDropFlags_None;

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar;

	ImGui::Begin("Content Browser", nullptr, windowFlags);

	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && editor_->lmbDownLastFrame)
	{
		ImGui::SetWindowFocus();
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("New folder", NULL, false, true))
			{
				std::filesystem::create_directory(
					currentDirectory_ / std::filesystem::path("new_folder"));
			}
			if (ImGui::MenuItem("New material", NULL, false, true))
			{
				AssetManager::Get().CreateDefaultMaterial(
					currentDirectory_.string().c_str());
			}
			if (ImGui::MenuItem("New script", NULL, false, true))
			{
				AssetManager::Get().CreateDefaultScript(
					currentDirectory_.string().c_str());
			}
			if (ImGui::MenuItem("Prefab from selected", NULL, false, true))
			{
				editor_->game_->OnSaveSelectedToPrefabPressed();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Import"))
		{
			if (ImGui::MenuItem("Import new asset", NULL, false, true))
			{
				editor_->game_->AssetFileImport(
					currentDirectory_.string().c_str());
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::Text(" Alt+LMB: delete, F2+LMB: rename folder");

	if (currentDirectory_ != rootDirectory_)
	{
		ImGui::PushID(0);
		if(ImGui::Button("<-"))
		{
			currentDirectory_ = currentDirectory_.parent_path();
		}
		if (ImGui::BeginDragDropTarget())
		{
			auto payload = ImGui::AcceptDragDropPayload("directory");
			if (payload)
			{
				auto payloadDir = (const char*)payload->Data;
				std::filesystem::rename(payloadDir, currentDirectory_.parent_path() /
							std::filesystem::path(payloadDir).filename());
			}
			else
			{
				payload = ImGui::AcceptDragDropPayload("asset");
				if (payload)
				{
					auto payloadAsset = (const char*)payload->Data;
					std::ifstream f(payloadAsset);
					json j = json::parse(f);
					f.close();
					if (j.contains("filename"))
					{
						std::filesystem::path assetFilePath =
							FilenameUtils::MetaFilenameToFilePath(
								j.at("filename"),
								std::filesystem::path(payloadAsset).parent_path());
						std::filesystem::rename(assetFilePath, currentDirectory_.parent_path() /
						                            j.at("filename"));
					}
					std::filesystem::rename(payloadAsset,
						currentDirectory_.parent_path() /
							std::filesystem::path(payloadAsset).filename());
				}
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::PopID();
	}

	static float padding = 12.0f;
	static float thumbnailSize = 64.0f;
	float cellSize = thumbnailSize + padding;

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);
	if (columnCount < 1)
	{
		columnCount = 1;
	}

	const float footer_height_to_reserve =
		ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	if (ImGui::BeginChild("Browser", ImVec2(0, -footer_height_to_reserve),
	                      ImGuiChildFlags_None,
	                      ImGuiWindowFlags_HorizontalScrollbar))
	{
		ImGui::Columns(columnCount, 0, false);

		int itemNum = 1;

		for (auto & directoryEntry : std::filesystem::directory_iterator(currentDirectory_))
		{
			ImGui::PushID(itemNum++);

			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, rootDirectory_);
			std::string fileNameString = relativePath.filename().string();
			std::string fileNameStringNoExtension = RemoveExtension(fileNameString);
			bool renamed = path == renamedItem;

			if (directoryEntry.is_directory())
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::ImageButton(TextureFactory::GetTextureHandle(
									   "2b7db204-d914-4d33-a4e4-dc7c7f9ff216"),
				                   {thumbnailSize, thumbnailSize}, {0, -1},
				                   {-1, 0});
				if (ImGui::BeginDragDropSource(dragDropFlags))
				{
					std::string path = directoryEntry.path().string();
					ImGui::SetDragDropPayload("directory", &path[0], path.size() + sizeof(std::string::value_type));
					ImGui::EndDragDropSource();
				}
				if (ImGui::BeginDragDropTarget())
				{
					auto payload = ImGui::AcceptDragDropPayload("directory");
					if (payload)
					{
						auto payloadDir = (const char*)payload->Data;
						if (payloadDir != directoryEntry.path().string())
						{
							std::filesystem::rename(
								payloadDir,
								directoryEntry.path() /
									std::filesystem::path(payloadDir).filename());
						}
					}
					else
					{
						payload = ImGui::AcceptDragDropPayload("asset");
						if (payload)
						{
							auto payloadAsset = (const char*)payload->Data;
							std::ifstream f(payloadAsset);
							json j = json::parse(f);
							f.close();
							if (j.contains("filename"))
							{
								std::filesystem::path assetFilePath =
									FilenameUtils::MetaFilenameToFilePath(
										j.at("filename"),
										std::filesystem::path(payloadAsset).parent_path());
								std::filesystem::rename(assetFilePath,
								                        directoryEntry.path() /
								                            j.at("filename"));
							}
							std::filesystem::rename(
								payloadAsset,
								directoryEntry.path() /
									std::filesystem::path(payloadAsset).filename());
						}
					}
					ImGui::EndDragDropTarget();
				}
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
						editor_->game_->SetContentBrowserFilePath(
							currentDirectory_.string());
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
				if (directoryEntry.path().extension().generic_string() ==
				    ".meta")
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

						ASSET_DRAG_N_DROP_SOURCE

						if (ImGui::IsItemHovered())
						{
							CONTENT_BROWSER_COMMON_ACTIONS

							if (ImGui::IsMouseDoubleClicked(
									ImGuiMouseButton_Left))
							{
								auto path = directoryEntry.path();
								path.remove_filename();
								path = path / scriptMetadata.filename;

								std::cout << std::flush;
								std::system(
									("code " + path.string())
								                .c_str());
							}
						}

						ImGui::PopStyleColor();

						ASSET_NAME_DISPLAY

						ImGui::NextColumn();
					}
					else if (metadata.at("type") == "world")
					{

						ImGui::ImageButton(
							TextureFactory::GetTextureHandle(
								"e204189e-5bb5-4fe3-a3b9-92fb27ab4c96"),
							{thumbnailSize, thumbnailSize}, {0, -1}, {-1, 0});

						ASSET_DRAG_N_DROP_SOURCE

						if (ImGui::IsItemHovered())
						{
							CONTENT_BROWSER_COMMON_ACTIONS

							if (ImGui::IsMouseDoubleClicked(
									ImGuiMouseButton_Left))
							{
								editor_->game_->ClearWorld();
								ComponentFactory::ResolveWorld(metadata);
							}
						}

						ImGui::PopStyleColor();

						ASSET_NAME_DISPLAY

						ImGui::NextColumn();
					}
					else if (metadata.at("type") == "mesh")
					{
						MeshMetaData meshMetaData;
						metadata.get_to(meshMetaData);

						ImGui::ImageButton(
							TextureFactory::GetTextureHandle(
								"e204189e-5bb5-4fe3-a3b9-92fb27ab4c96"),
							{thumbnailSize, thumbnailSize}, {0, -1}, {-1, 0});

						ASSET_DRAG_N_DROP_SOURCE

						if (ImGui::IsItemHovered())
						{
							CONTENT_BROWSER_COMMON_ACTIONS

							if (ImGui::IsMouseDoubleClicked(
									ImGuiMouseButton_Left))
							{
							}
						}

						ImGui::PopStyleColor();

						ASSET_NAME_DISPLAY

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

						ASSET_DRAG_N_DROP_SOURCE

						if (ImGui::IsItemHovered())
						{
							CONTENT_BROWSER_COMMON_ACTIONS

							if (ImGui::IsMouseDoubleClicked(
									ImGuiMouseButton_Left))
							{
								editor_->materialEditor_->OpenMaterial(
									directoryEntry.path().string());
							}
						}

						ImGui::PopStyleColor();

						ASSET_NAME_DISPLAY

						ImGui::NextColumn();
					}
					else if (metadata.at("type") == "texture2d")
					{
						Texture2DMetaData texture;
						metadata.get_to(texture);

						ImGui::ImageButton(
							TextureFactory::GetTextureHandle(
								"e204189e-5bb5-4fe3-a3b9-92fb27ab4c96"),
							{thumbnailSize, thumbnailSize}, {0, -1}, {-1, 0});

						ASSET_DRAG_N_DROP_SOURCE

						if (ImGui::IsItemHovered())
						{
							CONTENT_BROWSER_COMMON_ACTIONS

							if (ImGui::IsMouseDoubleClicked(
									ImGuiMouseButton_Left))
							{
								// TODO: display texture viewer
							}
						}

						ImGui::PopStyleColor();

						ASSET_NAME_DISPLAY

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

						ASSET_DRAG_N_DROP_SOURCE

						if (ImGui::IsItemHovered())
						{
							CONTENT_BROWSER_COMMON_ACTIONS

							if (ImGui::IsMouseDoubleClicked(
									ImGuiMouseButton_Left))
							{
							}
						}

						ImGui::PopStyleColor();

						ASSET_NAME_DISPLAY

						ImGui::NextColumn();
					}
					else
					{

						ImGui::ImageButton(
							TextureFactory::GetTextureHandle(
								"e204189e-5bb5-4fe3-a3b9-92fb27ab4c96"),
							{thumbnailSize, thumbnailSize}, {0, -1}, {-1, 0});

						ASSET_DRAG_N_DROP_SOURCE

						if (ImGui::IsItemHovered())
						{
							CONTENT_BROWSER_COMMON_ACTIONS

							if (ImGui::IsMouseDoubleClicked(
									ImGuiMouseButton_Left))
							{
								Debug::LogMessage("Double-clicked on Asset");
								// logic for any other asset
							}
						}

						ImGui::PopStyleColor();

						ASSET_NAME_DISPLAY

						ImGui::NextColumn();
					}
				}
			}
			ImGui::PopID();
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
