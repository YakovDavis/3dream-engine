#include "TextureFactory.h"
#include "D3E/Game.h"
#include "render/GameRender.h"
#include <filesystem>
#include "EASTL/string.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool D3E::TextureFactory::isInitialized_ = false;
D3E::Game* D3E::TextureFactory::activeGame_;
eastl::unordered_map<eastl::string, D3E::Texture> D3E::TextureFactory::textures_ {};

void D3E::TextureFactory::LoadTexture(const eastl::string& name, const eastl::string& fileName, nvrhi::DeviceHandle& device, nvrhi::CommandListHandle commandList)
{
	Debug::LogMessage("[TextureFactory] Loading texture file");
	Texture texture;

	texture.Filename = eastl::string(std::filesystem::current_path().string().c_str()) + "\\Textures\\" + fileName;

	Debug::LogMessage(texture.Filename);

	int width, height, comps;

	auto imageData = stbi_load(texture.Filename.c_str(), &width, &height, &comps, 4);

	if (!imageData)
	{
		Debug::LogError("[TextureFactory] Texture file not found");
		Debug::LogError(texture.Filename);
		return;
	}

	Debug::LogMessage("[TextureFactory] Texture file loaded");

	auto& textureDesc = nvrhi::TextureDesc()
	                        .setDimension(nvrhi::TextureDimension::Texture2D)
	                        //.setKeepInitialState( true )
	                        //.setInitialState( nvrhi::ResourceStates::Common | nvrhi::ResourceStates::ShaderResource )
	                        .setWidth(width)
	                        .setHeight(height)
	                        .setFormat(nvrhi::Format::RGBA8_UNORM);

	Debug::LogMessage("[TextureFactory] Texture desc created");

	texture.Handle = device->createTexture(textureDesc);

	Debug::LogMessage("[TextureFactory] Texture handle created");

	commandList->open();
	commandList->beginTrackingTextureState( texture.Handle, nvrhi::AllSubresources, nvrhi::ResourceStates::Common);
	commandList->writeTexture(texture.Handle, 0, 0, imageData, width * comps * 1);
	commandList->setPermanentTextureState(texture.Handle, nvrhi::ResourceStates::ShaderResource);
	commandList->close();

	device->executeCommandList(commandList);

	textures_.insert({name, texture});
	Debug::LogMessage("[TextureFactory] Texture loaded");
}

void D3E::TextureFactory::Initialize(Game* game)
{
}

void D3E::TextureFactory::DestroyResources()
{
}

nvrhi::TextureHandle D3E::TextureFactory::GetTextureHandle(const eastl::string& name)
{
	if (textures_.find(name) == textures_.end())
	{
		Debug::LogError("[TextureFactory] Texture handle not found");
	}

	//LoadTexture(name);

	return textures_[name].Handle;
}
