#pragma once

#include "D3E/CommonHeader.h"
#include "D3E/render/Material.h"
#include "EASTL/string.h"
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

	private:
		AssetManager() = default;

		static AssetManager instance_;

	public:
		AssetManager(AssetManager const&) = delete;
		void operator=(AssetManager const&) = delete;
	};
}
