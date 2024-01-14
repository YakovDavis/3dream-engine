#include "D3E/AssetManager.h"

#include "D3E/CommonCpp.h"
#include "D3E/Debug.h"
#include "D3E/Uuid.h"
#include "D3E/render/Material.h"
#include "MaterialFactory.h"
#include "MeshFactory.h"
#include "MeshMetaData.h"
#include "ScriptFactory.h"
#include "ScriptMetaData.h"
#include "SoundMetaData.h"
#include "Texture2DMetaData.h"
#include "TextureFactory.h"
#include "json.hpp"
#include "sound_engine/SoundEngine.h"
#include "utils/FilenameUtils.h"
#include "uuid_v4.h"

#include <filesystem>
#include <fstream>

using json = nlohmann::json;

UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;

D3E::AssetManager D3E::AssetManager::instance_ = {};

eastl::unordered_map<D3E::String, D3E::String> D3E::AssetManager::assetMetaData_ = {};

D3E::AssetManager& D3E::AssetManager::Get()
{
	return instance_;
}

void D3E::AssetManager::LoadAssetsInFolder(const String& folder, bool recursive,
                                           nvrhi::IDevice* device,
                                           nvrhi::ICommandList* commandList)
{
	if (!std::filesystem::exists(folder.c_str()))
	{
		Debug::LogError(
			std::format("[AssetManager] Path: {} does not exist!", folder.c_str()).c_str());
		return;
	}

	for (const auto& entry :
	     std::filesystem::directory_iterator(folder.c_str()))
	{
		if (entry.is_directory() && recursive)
		{
			LoadAssetsInFolder(String(entry.path().string().c_str()), recursive, device, commandList);
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
			f.close();

			if (metadata.at("type") == "texture2d")
			{
				Texture2DMetaData asset;
				metadata.get_to(asset);
				assetMetaData_.insert({String(asset.uuid.c_str()), String(asset.name.c_str())});
				TextureFactory::LoadTexture(asset, folder.c_str(), false, device, commandList);

				continue;
			}

			if (metadata.at("type") == "mesh")
			{
				MeshMetaData asset;
				metadata.get_to(asset);
				assetMetaData_.insert({String(asset.uuid.c_str()), String(asset.name.c_str())});
				MeshFactory::LoadMesh(asset, folder.c_str(), false, device, commandList);

				continue;
			}

			if (metadata.at("type") == "script")
			{
				ScriptMetaData asset;
				metadata.get_to(asset);
				assetMetaData_.insert(
					{String(asset.uuid.c_str()), String(asset.filename.c_str())});
				ScriptFactory::LoadScript(asset, folder.c_str());

				continue;
			}

			if (metadata.at("type") == "material")
			{
				Material asset;
				metadata.get_to(asset);
				assetMetaData_.insert({String(asset.uuid.c_str()), String(asset.name.c_str())});
				MaterialFactory::AddMaterial(asset);

				continue;
			}

			if (metadata.at("type") == "sound")
			{
				SoundMetaData asset;
				metadata.get_to(asset);
				assetMetaData_.insert({String(asset.uuid.c_str()), String(asset.name.c_str())});
				SoundEngine::GetInstance().LoadSound(asset, folder.c_str());

				continue;
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
	asset.filename = std::filesystem::path(filename.c_str()).filename().string();
	asset.name = name.c_str();

	TextureFactory::LoadTexture(asset, std::filesystem::path(filename.c_str()).parent_path().string(), true, device, commandList);

	json j(asset);
	std::ofstream o(std::filesystem::path(filename.c_str()).replace_filename(asset.name + ".meta"));
	o << std::setw(4) << j << std::endl;
	o.close();
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

	MeshFactory::LoadMesh(asset, std::filesystem::path(filename.c_str()).parent_path().string(), true, device, commandList);

	json j(asset);
	std::ofstream o(std::filesystem::path(filename.c_str()).replace_filename(asset.name + ".meta"));
	o << std::setw(4) << j << std::endl;
	o.close();
}

void D3E::AssetManager::CreateMaterial(D3E::Material& material,
                                       const std::string& folder)
{
	material.uuid = uuidGenerator.getUUID().str().c_str();

	MaterialFactory::AddMaterial(material);

	json j(material);
	std::ofstream o(folder + "\\" + std::string(material.name.c_str()) + ".meta");
	o << std::setw(4) << j << std::endl;
	o.close();
}

void D3E::AssetManager::CreateSound(const D3E::String& name,
                                    const D3E::String& filename, bool is3d,
                                    bool isLooping, bool stream)
{
	SoundMetaData asset;
	asset.uuid = uuidGenerator.getUUID().str();
	asset.filename = filename.c_str();
	asset.name = name.c_str();

	SoundEngine::GetInstance().LoadSound(asset, std::filesystem::path(filename.c_str()).parent_path().string());

	json j(asset);
	std::ofstream o(std::filesystem::path(filename.c_str()).replace_filename(asset.name + ".meta"));
	o << std::setw(4) << j << std::endl;
	o.close();
}

bool D3E::AssetManager::IsExtensionTexture(const std::string& name)
{
	if (name == ".png")
	{
		return true;
	}
	return false;
}

bool D3E::AssetManager::IsExtensionModel(const std::string& name)
{
	if (name == ".obj" || name == ".fbx")
	{
		return true;
	}
	return false;
}

bool D3E::AssetManager::IsExtensionSound(const std::string& name)
{
	if (name == ".mp3" || name == ".wav")
	{
		return true;
	}
	return false;
}

void D3E::AssetManager::DeleteAsset(const D3E::String& filename)
{
	if (!std::filesystem::exists(filename.c_str()))
	{
		Debug::LogError(
			std::format("[AssetManager] Path: {} does not exist!", filename.c_str()).c_str());
		return;
	}

	std::ifstream f(filename.c_str());
	json metadata = json::parse(f);
	f.close();

	if (metadata.contains("filename"))
	{
		std::filesystem::remove(metadata.at("filename"));
	}

	std::filesystem::remove(filename.c_str());
}

D3E::String D3E::AssetManager::GetAssetName(const D3E::String& uuid)
{
	return assetMetaData_.at(uuid);
}

void D3E::AssetManager::CreateDefaultMaterial(const std::string& folder)
{
	Material m = {};
	m.type = MaterialType::Lit;
	m.name = "NewMaterial";
	m.albedoTextureUuid = kDebugLinesTextureUUID;
	m.roughnessTextureUuid = kBlackTextureUUID;
	m.metalnessTextureUuid = kBlackTextureUUID;
	m.normalTextureUuid = kNormalsDefaultTextureUUID;
	CreateMaterial(m, folder);
}
