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

		void CreateTexture(const String& name, const String& filename, nvrhi::IDevice* device, nvrhi::ICommandList* commandList);

		void CreateMesh(const String& name, const String& filename, nvrhi::IDevice* device, nvrhi::ICommandList* commandList);

		void CreateMaterial(Material& material, const std::string& folder);

		void CreateDefaultMaterial(const std::string& folder);

		void CreateSound(const String& name, const String& filename, bool is3d = false, bool isLooping = false, bool stream = false);

		void DeleteAsset(const String& filename); // filename of corresponding metafile

		static bool IsExtensionTexture(const std::string& name);

		static bool IsExtensionModel(const std::string& name);

		static bool IsExtensionSound(const std::string& name);

		static String GetAssetName(const String& uuid);

		static void RegisterExternalAssetName(const String& uuid, const String& name);

		static bool IsPrefabUuidValid(const String& uuid);

		static std::string GetPrefabFilePath(const String& uuid);

	private:
		AssetManager() = default;

		static AssetManager instance_;

		static eastl::unordered_map<String, String> assetMetaData_;

		static eastl::unordered_map<String, std::string> prefabsMap_;

	public:
		AssetManager(AssetManager const&) = delete;
		void operator=(AssetManager const&) = delete;
	};
}
