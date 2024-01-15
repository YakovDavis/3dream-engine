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

eastl::unordered_map<D3E::String, json> D3E::AssetManager::prefabsMap_ = {};

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
				LoadTexture(metadata, folder, device, commandList);
				continue;
			}

			if (metadata.at("type") == "mesh")
			{
				LoadMesh(metadata, folder, device, commandList);
				continue;
			}

			if (metadata.at("type") == "script")
			{
				LoadScript(metadata, folder);
				continue;
			}

			if (metadata.at("type") == "material")
			{
				LoadMaterial(metadata);
				continue;
			}

			if (metadata.at("type") == "sound")
			{
				LoadSound(metadata, folder);
				continue;
			}

			if (metadata.at("type") == "entity")
			{
				LoadPrefab(metadata, entry.path().string());
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

	InsertOrReplaceAssetName(asset.uuid.c_str(), asset.name.c_str());
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
	asset.filename = std::filesystem::path(filename.c_str()).filename().string();
	asset.name = name.c_str();

	InsertOrReplaceAssetName(asset.uuid.c_str(), asset.name.c_str());
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

	InsertOrReplaceAssetName(material.uuid.c_str(), material.name.c_str());
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

	InsertOrReplaceAssetName(asset.uuid.c_str(), asset.name.c_str());
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

	if (metadata.contains("type") && metadata.contains("uuid"))
	{
		if (metadata.at("type") == "texture2d")
		{
			TextureFactory::UnloadTexture(std::string(metadata.at("uuid")).c_str());
		}
		if (metadata.at("type") == "mesh")
		{
			MeshFactory::UnloadMesh(std::string(metadata.at("uuid")).c_str());
		}
		if (metadata.at("type") == "script")
		{
			// TODO
		}
		if (metadata.at("type") == "material")
		{
			MaterialFactory::RemoveMaterial(std::string(metadata.at("uuid")).c_str());
		}
		if (metadata.at("type") == "sound")
		{
			SoundEngine::GetInstance().UnloadSound(std::string(metadata.at("uuid")).c_str());
		}
		if (metadata.at("type") == "entity")
		{
			if (prefabsMap_.find(std::string(metadata.at("uuid")).c_str()) != prefabsMap_.end())
			{
				prefabsMap_.erase(std::string(metadata.at("uuid")).c_str());
			}
		}
	}

	if (metadata.contains("filename"))
	{
		std::filesystem::remove(FilenameUtils::MetaFilenameToFilePath(metadata.at("filename"), std::filesystem::path(filename.c_str()).parent_path().string()));
	}

	std::filesystem::remove(filename.c_str());
}

D3E::String D3E::AssetManager::GetAssetName(const D3E::String& uuid)
{
	if (assetMetaData_.find(uuid) == assetMetaData_.end())
	{
		return "";
	}
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

bool D3E::AssetManager::IsPrefabUuidValid(const D3E::String& uuid)
{
	return prefabsMap_.find(uuid) != prefabsMap_.end();
}

const json& D3E::AssetManager::GetPrefab(const D3E::String& uuid)
{
	if (!IsPrefabUuidValid(uuid))
	{
		return "";
	}

	return prefabsMap_[uuid];
}

void D3E::AssetManager::RegisterExternalAssetName(const D3E::String& uuid,
                                             const D3E::String& name)
{
	assetMetaData_.insert({uuid, name});
}

void D3E::AssetManager::LoadTexture(const json& meta, const D3E::String& folder,
                                    nvrhi::IDevice* device,
                                    nvrhi::ICommandList* commandList)
{
	Texture2DMetaData asset;
	meta.get_to(asset);
	InsertOrReplaceAssetName(asset.uuid.c_str(), asset.name.c_str());
	TextureFactory::LoadTexture(asset, folder.c_str(), false, device, commandList);
}

void D3E::AssetManager::LoadMesh(const json& meta, const D3E::String& folder,
                                 nvrhi::IDevice* device,
                                 nvrhi::ICommandList* commandList)
{
	MeshMetaData asset;
	meta.get_to(asset);
	InsertOrReplaceAssetName(asset.uuid.c_str(), asset.name.c_str());
	MeshFactory::LoadMesh(asset, folder.c_str(), false, device, commandList);
}

void D3E::AssetManager::LoadMaterial(const json& meta)
{
	Material asset;
	meta.get_to(asset);
	InsertOrReplaceAssetName(asset.uuid.c_str(), asset.name.c_str());
	MaterialFactory::AddMaterial(asset);
}

void D3E::AssetManager::LoadSound(const json& meta, const String& folder)
{
	SoundMetaData asset;
	meta.get_to(asset);
	InsertOrReplaceAssetName(asset.uuid.c_str(), asset.name.c_str());
	SoundEngine::GetInstance().LoadSound(asset, folder.c_str());
}

void D3E::AssetManager::LoadPrefab(const json& meta, const std::string& path)
{
	String uuid = std::string(meta.at("uuid")).c_str();
	std::ifstream f(path);
	json j = json::parse(f);
	f.close();
	if (prefabsMap_.find(std::string(meta.at("uuid")).c_str()) == prefabsMap_.end())
	{
		prefabsMap_.insert(uuid);
		prefabsMap_[uuid] = j;
	}
	else
	{
		prefabsMap_[uuid] = j;
	}
}

void D3E::AssetManager::LoadScript(const json& meta, const D3E::String& folder)
{
	ScriptMetaData asset;
	meta.get_to(asset);
	InsertOrReplaceAssetName(asset.uuid.c_str(), asset.filename.c_str());
	ScriptFactory::LoadScript(asset, folder.c_str());
}

void D3E::AssetManager::InsertOrReplaceAssetName(const D3E::String& uuid,
                                                 const D3E::String& name)
{
	if (assetMetaData_.find(uuid) == assetMetaData_.end())
	{
		assetMetaData_.insert({uuid, name});
	}
	else
	{
		assetMetaData_[uuid] = name;
	}
}
