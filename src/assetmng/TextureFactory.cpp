#include "TextureFactory.h"

#include "D3E/CommonCpp.h"
#include "D3E/Game.h"
#include "render/GameRender.h"
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool D3E::TextureFactory::isInitialized_ = false;
D3E::Game* D3E::TextureFactory::activeGame_;
D3E::GBuffer* D3E::TextureFactory::gbuffer_;
eastl::unordered_map<D3E::String, D3E::Texture> D3E::TextureFactory::textures_ {};
eastl::unordered_map<D3E::String, nvrhi::SamplerHandle> D3E::TextureFactory::samplers_ {};

void D3E::TextureFactory::LoadTexture(Texture2DMetaData& metaData, bool firstLoad, nvrhi::IDevice* device, nvrhi::ICommandList* commandList)
{
	Texture texture;

	texture.MetaData = metaData;

	Debug::LogMessage("[TextureFactory] Loading texture file " + eastl::string(texture.MetaData.filename.c_str()));

	int width, height, comps;

	auto imageData = stbi_load(texture.MetaData.filename.c_str(), &width, &height, &comps, 4);

	if (!imageData)
	{
		Debug::LogError("[TextureFactory] Texture file not found");
		Debug::LogError(texture.MetaData.filename.c_str());
		return;
	}

	if (firstLoad)
	{
		metaData.format.channels = TextureChannels::RGBA8; // TODO: support other texture types
	}

	if (width != metaData.format.dimensions[0] || height != metaData.format.dimensions[1])
	{
		if (!firstLoad)
		{
			Debug::LogWarning("[TextureFactory] Texture metadata contains incorrect texture dimensions, adjusting");
		}
		metaData.format.dimensions[0] = width;
		metaData.format.dimensions[1] = height;
	}

	auto& textureDesc = nvrhi::TextureDesc()
	                        .setDimension(nvrhi::TextureDimension::Texture2D)
	                        //.setKeepInitialState( true )
	                        //.setInitialState( nvrhi::ResourceStates::Common | nvrhi::ResourceStates::ShaderResource )
	                        .setWidth(width)
	                        .setHeight(height)
	                        .setFormat(nvrhi::Format::RGBA8_UNORM);

	texture.Handle = device->createTexture(textureDesc);

	commandList->open();
	commandList->beginTrackingTextureState( texture.Handle, nvrhi::AllSubresources, nvrhi::ResourceStates::Common);
	commandList->writeTexture(texture.Handle, 0, 0, imageData, width * comps * 1);
	commandList->setPermanentTextureState(texture.Handle, nvrhi::ResourceStates::ShaderResource);
	commandList->close();

	device->executeCommandList(commandList);

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
