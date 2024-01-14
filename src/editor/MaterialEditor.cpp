#include "MaterialEditor.h"

#include "D3E/AssetManager.h"
#include "D3E/render/Material.h"
#include "Editor.h"
#include "EditorContentBrowser.h"
#include "assetmng/TextureFactory.h"
#include "json.hpp"
#include "misc/cpp/imgui_stdlib.h"

static D3E::Material currentMaterialSaved = {};

D3E::MaterialEditor::MaterialEditor(D3E::Editor* editor)
{
	editor_ = editor;
	open = false;
}

void D3E::MaterialEditor::Draw()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;

	ImGui::SetNextWindowSize(ImVec2(400, 200));

	ImGui::Begin("Material Editor", nullptr, flags);

	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && editor_->lmbDownLastFrame)
	{
		ImGui::SetWindowFocus();
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			if (ImGui::MenuItem("Save material", NULL, false, true))
			{
				SaveMaterial();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Close", NULL, false, true))
			{
				open = false;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_ReadOnly;

	static int selectedMaterialType = 0;
	ImGui::Text("Material type: ");
	ImGui::SameLine();
	ImGui::Combo("##material_type", &selectedMaterialType, "Lit\0\0");

	ImGui::Spacing();

	ImGui::Text("Albedo texture: ");
	ImGui::SameLine();
	ImGui::InputText("##albedo_texture", &albedoInputField, input_text_flags);
	if(ImGui::BeginDragDropTarget())
	{
		auto payload = ImGui::AcceptDragDropPayload("asset");
		if (payload)
		{
			auto payloadAsset = (const char*)payload->Data;
			std::ifstream f(payloadAsset);
			json j = json::parse(f);
			f.close();
			if (j.contains("uuid"))
			{
				std::string selectedUuid = j.at("uuid");
				if (TextureFactory::IsTextureUuidValid(selectedUuid.c_str()))
				{
					currentMaterialSaved.albedoTextureUuid = selectedUuid.c_str();
					albedoInputField = AssetManager::GetAssetName(currentMaterialSaved.albedoTextureUuid).c_str();
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::Spacing();

	ImGui::Text("Normal texture: ");
	ImGui::SameLine();
	ImGui::InputText("##normal_texture", &normalInputField, input_text_flags);
	if(ImGui::BeginDragDropTarget())
	{
		auto payload = ImGui::AcceptDragDropPayload("asset");
		if (payload)
		{
			auto payloadAsset = (const char*)payload->Data;
			std::ifstream f(payloadAsset);
			json j = json::parse(f);
			f.close();
			if (j.contains("uuid"))
			{
				std::string selectedUuid = j.at("uuid");
				if (TextureFactory::IsTextureUuidValid(selectedUuid.c_str()))
				{
					currentMaterialSaved.normalTextureUuid = selectedUuid.c_str();
					normalInputField = AssetManager::GetAssetName(currentMaterialSaved.normalTextureUuid).c_str();
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::Spacing();

	ImGui::Text("Metal texture: ");
	ImGui::SameLine();
	ImGui::InputText("##metal_texture", &metalnessInputField, input_text_flags);
	if(ImGui::BeginDragDropTarget())
	{
		auto payload = ImGui::AcceptDragDropPayload("asset");
		if (payload)
		{
			auto payloadAsset = (const char*)payload->Data;
			std::ifstream f(payloadAsset);
			json j = json::parse(f);
			f.close();
			if (j.contains("uuid"))
			{
				std::string selectedUuid = j.at("uuid");
				if (TextureFactory::IsTextureUuidValid(selectedUuid.c_str()))
				{
					currentMaterialSaved.metalnessTextureUuid = selectedUuid.c_str();
					metalnessInputField = AssetManager::GetAssetName(currentMaterialSaved.metalnessTextureUuid).c_str();
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::Spacing();

	ImGui::Text("Roughness texture: ");
	ImGui::SameLine();
	ImGui::InputText("##roughness_texture", &roughnessInputField, input_text_flags);
	if(ImGui::BeginDragDropTarget())
	{
		auto payload = ImGui::AcceptDragDropPayload("asset");
		if (payload)
		{
			auto payloadAsset = (const char*)payload->Data;
			std::ifstream f(payloadAsset);
			json j = json::parse(f);
			f.close();
			if (j.contains("uuid"))
			{
				std::string selectedUuid = j.at("uuid");
				if (TextureFactory::IsTextureUuidValid(selectedUuid.c_str()))
				{
					currentMaterialSaved.roughnessTextureUuid = selectedUuid.c_str();
					roughnessInputField = AssetManager::GetAssetName(currentMaterialSaved.roughnessTextureUuid).c_str();
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::End();
}

void D3E::MaterialEditor::OpenMaterial(const std::string& fname)
{
	filename = fname;
	std::ifstream f(filename);
	currentMaterialSaved = json::parse(f);
	f.close();
	albedoInputField = currentMaterialSaved.albedoTextureUuid != EmptyIdString ? AssetManager::GetAssetName(currentMaterialSaved.albedoTextureUuid).c_str() : "";
	normalInputField = currentMaterialSaved.normalTextureUuid != EmptyIdString ? AssetManager::GetAssetName(currentMaterialSaved.normalTextureUuid).c_str() : "";
	metalnessInputField = currentMaterialSaved.metalnessTextureUuid != EmptyIdString ? AssetManager::GetAssetName(currentMaterialSaved.metalnessTextureUuid).c_str() : "";
	roughnessInputField = currentMaterialSaved.roughnessTextureUuid != EmptyIdString ? AssetManager::GetAssetName(currentMaterialSaved.roughnessTextureUuid).c_str() : "";
	open = true;
}

void D3E::MaterialEditor::SaveMaterial()
{
	json j = json(currentMaterialSaved);
	std::ofstream f(filename);
	f << std::setw(4) << j << std::endl;
	f.close();
}
