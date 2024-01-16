#include "TextureFactory.h"

#include "D3E/CommonCpp.h"
#include "D3E/Game.h"
#include "render/GameRender.h"
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "utils/FilenameUtils.h"

bool D3E::TextureFactory::isInitialized_ = false;
D3E::Game* D3E::TextureFactory::activeGame_;
D3E::GBuffer* D3E::TextureFactory::gbuffer_;
eastl::unordered_map<D3E::String, D3E::Texture> D3E::TextureFactory::textures_ {};
eastl::unordered_map<D3E::String, nvrhi::SamplerHandle> D3E::TextureFactory::samplers_ {};

struct TextureSubresourceData
{
	size_t rowPitch = 0;
	size_t depthPitch = 0;
	ptrdiff_t dataOffset = 0;
	size_t dataSize = 0;
};

void D3E::TextureFactory::LoadTexture(Texture2DMetaData& metaData, const std::string& directory, bool firstLoad, nvrhi::IDevice* device, nvrhi::ICommandList* commandList)
{
	UnloadTexture(metaData.uuid.c_str());

	Texture texture;

	texture.MetaData = metaData;

	Debug::LogMessage("[TextureFactory] Loading texture file " + eastl::string(texture.MetaData.filename.c_str()));

	nvrhi::Format textureFormat;
	int bytesPerComponent = 1;
	switch (metaData.format.channels)
	{
		case TextureChannels::RGBA8:
			textureFormat = nvrhi::Format::RGBA8_UNORM;
			break;
		case TextureChannels::RGBA16:
			textureFormat = nvrhi::Format::RGBA16_UNORM;
			bytesPerComponent = 2;
			break;
	}

	int width, height, comps;

	void* imageData;
	if (bytesPerComponent == 2)
	{
		imageData = stbi_load_16(FilenameUtils::MetaFilenameToFilePath(
								  texture.MetaData.filename, directory)
		                          .string()
		                          .c_str(),
		                       &width, &height, &comps, STBI_default);
	}
	else
	{
		imageData = stbi_load(FilenameUtils::MetaFilenameToFilePath(
								  texture.MetaData.filename, directory)
		                          .string()
		                          .c_str(),
		                       &width, &height, &comps, STBI_default);
	}
	if (!imageData)
	{
		Debug::LogError("[TextureFactory] Texture file not found");
		Debug::LogError(texture.MetaData.filename.c_str());
		return;
	}

	if (firstLoad)
	{
		metaData.format.channels = TextureChannels::RGBA8; // TODO: detect other texture types
	}

	if (firstLoad && (width != metaData.format.dimensions[0] || height != metaData.format.dimensions[1]))
	{
		metaData.format.dimensions[0] = width;
		metaData.format.dimensions[1] = height;
	}

	nvrhi::TextureDesc textureDesc;

	if (metaData.format.type == Texture2D)
	{
		textureDesc.setDimension(nvrhi::TextureDimension::Texture2D);
		//textureDesc.setKeepInitialState( true )
		//.setInitialState( nvrhi::ResourceStates::Common | nvrhi::ResourceStates::ShaderResource )
		textureDesc.setWidth(metaData.format.dimensions[0]);
		textureDesc.setHeight(metaData.format.dimensions[1]);
		textureDesc.setFormat(textureFormat);
	}
	else if (metaData.format.type == TextureCube)
	{
		textureDesc.setDimension(nvrhi::TextureDimension::TextureCube);
		textureDesc.setArraySize(6);
		textureDesc.setWidth(metaData.format.dimensions[0]);
		textureDesc.setHeight(metaData.format.dimensions[1]);
		textureDesc.setFormat(textureFormat);
		//textureDesc.setIsUAV(true);
		//textureDesc.setInitialState( nvrhi::ResourceStates::Common);
	}

	texture.Handle = device->createTexture(textureDesc);

	if (metaData.lockResourceState)
	{
		commandList->open();
		commandList->beginTrackingTextureState(texture.Handle,
		                                       nvrhi::AllSubresources,
		                                       nvrhi::ResourceStates::Common);
		commandList->writeTexture(texture.Handle, 0, 0, imageData,
		                          metaData.format.dimensions[0] * comps * bytesPerComponent);
		commandList->setPermanentTextureState(
			texture.Handle, nvrhi::ResourceStates::ShaderResource);
		commandList->close();
		device->executeCommandList(commandList);
	}
	else
	{
		commandList->open();
		commandList->beginTrackingTextureState(texture.Handle,
		                                       nvrhi::AllSubresources,
		                                       nvrhi::ResourceStates::Common);
		for (uint32_t arraySlice = 0; arraySlice < textureDesc.arraySize; arraySlice++)
		{
			commandList->writeTexture(texture.Handle, arraySlice, 0, static_cast<const char*>(imageData) + arraySlice * metaData.format.dimensions[0] * metaData.format.dimensions[1] * comps * bytesPerComponent, metaData.format.dimensions[0] * comps * bytesPerComponent);
		}
		commandList->setPermanentTextureState(
			texture.Handle, nvrhi::ResourceStates::ShaderResource);
		commandList->close();
		device->executeCommandList(commandList);
	}

	textures_.insert({texture.MetaData.uuid.c_str(), texture});
}

void D3E::TextureFactory::Initialize(Game* game)
{
}

void D3E::TextureFactory::DestroyResources()
{
}

nvrhi::TextureHandle D3E::TextureFactory::GetTextureHandle(const String& uuid)
{
	if (textures_.find(uuid) == textures_.end())
	{
		Debug::LogError("[TextureFactory] Texture handle not found: " + uuid);
	}

	return textures_[uuid].Handle;
}

nvrhi::SamplerHandle& D3E::TextureFactory::GetSampler(const String& name)
{
	if (samplers_.find(name) == samplers_.end())
	{
		Debug::LogError("[TextureFactory] Sampler handle not found");
	}
	return samplers_[name];
}

nvrhi::SamplerHandle& D3E::TextureFactory::AddSampler(const String& name, nvrhi::IDevice* device,
                                const nvrhi::SamplerDesc& desc)
{
	samplers_.insert({name, device->createSampler(desc)});
	return samplers_[name];
}

bool D3E::TextureFactory::IsTextureUuidValid(const D3E::String& uuid)
{
	return textures_.find(uuid) != textures_.end();
}

void D3E::TextureFactory::UnloadTexture(const D3E::String& uuid)
{
	if (textures_.find(uuid) == textures_.end())
	{
		return;
	}
	textures_[uuid].Handle.Reset();
	textures_.erase(uuid);
}

void D3E::TextureFactory::RenameTexture(const D3E::String& uuid,
                                        const D3E::String& name)
{
	if (textures_.find(uuid) == textures_.end())
	{
		return;
	}
	textures_[uuid].MetaData.name = name.c_str();
}

nvrhi::TextureHandle D3E::TextureFactory::GetNewTextureHandle(const D3E::String& uuid)
{
	nvrhi::TextureHandle newHandle;
	textures_.insert({uuid, {newHandle, {}}});
	return newHandle;
}
