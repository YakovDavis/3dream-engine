#include "D3E/AssetManager.h"

#include "D3E/CommonCpp.h"
#include "D3E/Debug.h"
#include "MeshFactory.h"
#include "MeshMetaData.h"
#include "Texture2DMetaData.h"
#include "TextureFactory.h"
#include "json.hpp"
#include "uuid_v4.h"

#include <filesystem>
#include <fstream>

using json = nlohmann::json;

UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;

D3E::AssetManager D3E::AssetManager::instance_ = {};

D3E::AssetManager& D3E::AssetManager::Get()
{
	return instance_;
}

void D3E::AssetManager::LoadAssetsInFolder(const String& folder,
                                           bool recursive,
                                           nvrhi::IDevice* device,
                                           nvrhi::ICommandList* commandList)
{
	for (const auto & entry : std::filesystem::directory_iterator(folder.c_str()))
	{
		if (entry.is_directory() && recursive)
		{
			LoadAssetsInFolder(folder, recursive, device, commandList);
			continue;
		}
		else if (entry.is_directory() && !recursive)
		{
			continue;
		}

		if (entry.path().extension().generic_string() == ".meta")
		{
			std::ifstream f(entry.path());
			json metadata = json::parse(f);
			if (metadata.at("type") == "texture2d")
			{
				Texture2DMetaData asset;
				metadata.get_to(asset);
				TextureFactory::LoadTexture(asset, false, device, commandList);
			}
			if (metadata.at("type") == "mesh")
			{
				MeshMetaData asset;
				metadata.get_to(asset);
				MeshFactory::LoadMesh(asset, false, device, commandList);
			}
		}
	}
}

void D3E::AssetManager::CreateTexture(const D3E::String& name,
                                      const D3E::String& filename,
                                      nvrhi::IDevice* device,
                                      nvrhi::ICommandList* commandList)
{
	Texture2DMetaData asset;
	asset.uuid = uuidGenerator.getUUID().str();
	asset.filename = filename.c_str();
	asset.name = name.c_str();

	TextureFactory::LoadTexture(asset, true, device, commandList);

	json j(asset);
	const size_t last_slash_idx = filename.rfind('/');
	std::string dir = "";
	if (std::string::npos != last_slash_idx)
	{
		dir = filename.substr(0, last_slash_idx).c_str();
	}
	std::ofstream o(dir + "/" + asset.name + ".meta");
	o << std::setw(4) << j << std::endl;
}

void D3E::AssetManager::CreateMesh(const D3E::String& name,
                                   const D3E::String& filename,
                                   nvrhi::IDevice* device,
                                   nvrhi::ICommandList* commandList)
{
	MeshMetaData asset;
	asset.uuid = uuidGenerator.getUUID().str();
	asset.filename = filename.c_str();
	asset.name = name.c_str();

	MeshFactory::LoadMesh(asset, true, device, commandList);

	json j(asset);
	const size_t last_slash_idx = filename.rfind('/');
	std::string dir = "";
	if (std::string::npos != last_slash_idx)
	{
		dir = filename.substr(0, last_slash_idx).c_str();
	}
	std::ofstream o(dir + "/" + asset.name + ".meta");
	o << std::setw(4) << j << std::endl;
}
