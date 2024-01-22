#pragma once

#include "D3E/CommonHeader.h"
#include "D3E/render/Material.h"
#include "EASTL/string.h"
#include "EASTL/unordered_map.h"
#include "nvrhi/nvrhi.h"

namespace D3E
{
	class AssetManager
	{
	public:
		static AssetManager& Get();

		void LoadAssetsInFolder(const String& folder, bool recursive, nvrhi::IDevice* device, nvrhi::ICommandList* commandList);

		void LoadScripts(const String& folder);

		void LoadTexture(const json& meta, const String& folder, nvrhi::IDevice* device, nvrhi::ICommandList* commandList);

		void LoadMesh(const json& meta, const String& folder, nvrhi::IDevice* device, nvrhi::ICommandList* commandList);

		void LoadMaterial(const json& meta);

		void LoadSound(const json& meta, const String& folder);

		void LoadPrefab(const json& meta, const std::string& path);

		void LoadScript(const json& meta, const String& folder);

		void CreateTexture(const String& name, const String& filename, nvrhi::IDevice* device, nvrhi::ICommandList* commandList);

		void CreateMesh(const String& name, const String& filename, nvrhi::IDevice* device, nvrhi::ICommandList* commandList);

		void CreateMaterial(Material& material, const std::string& folder);

		void CreateDefaultMaterial(const std::string& folder);

		void CreateDefaultScript(const std::string& folder);

		void CreateSound(const String& name, const String& filename, bool is3d = false, bool isLooping = false, bool stream = false);

		void DeleteAsset(const String& filename); // filename of corresponding metafile

		static bool IsExtensionTexture(const std::string& name);

		static bool IsExtensionModel(const std::string& name);

		static bool IsExtensionSound(const std::string& name);

		static String GetAssetName(const String& uuid);

		static void RegisterExternalAssetName(const String& uuid, const String& name);

		static bool IsPrefabUuidValid(const String& uuid);

		static const json& D3E::AssetManager::GetPrefab(const String& uuid);

		static void InsertOrReplaceAssetName(const String& uuid, const String& name);

	private:
		AssetManager() = default;

		static AssetManager instance_;

		static eastl::unordered_map<String, String> assetMetaData_;

		static eastl::unordered_map<String, json> prefabsMap_;

	public:
		AssetManager(AssetManager const&) = delete;
		void operator=(AssetManager const&) = delete;
	};
}
