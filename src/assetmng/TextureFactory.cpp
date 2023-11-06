#include "TextureFactory.h"
#include "D3E/Game.h"
#include "render/GameRender.h"
#include "DirectXTex.h"

bool D3E::TextureFactory::isInitialized_ = false;
D3E::Game* D3E::TextureFactory::activeGame_;
eastl::unordered_map<eastl::string, nvrhi::TextureHandle> D3E::TextureFactory::textures_ {};

void D3E::TextureFactory::LoadTexture(const eastl::string& name)
{
	/*WCHAR* filePath = L"YourPathHere";
	auto *imageData = new DirectX::ScratchImage();
	HRESULT loadResult = DirectX::LoadFromDDSFile(filePath, DirectX::DDS_FLAGS_NONE, nullptr, *imageData);
	assert(loadResult == S_OK);

	const DirectX::TexMetadata& textureMetaData = imageData->GetMetadata();
	DXGI_FORMAT textureFormat = textureMetaData.format;

	nvrhi::TextureDesc textureDesc = {};
	textureDesc.format
	activeGame_->GetRender()->GetDevice()->createHandleForNativeTexture(textureDesc);*/
}

void D3E::TextureFactory::Initialize(Game* game)
{
}

void D3E::TextureFactory::DestroyResources()
{
}

nvrhi::TextureHandle D3E::TextureFactory::GetTexture(const eastl::string& name)
{
	if (textures_.find(name) != textures_.end())
		return textures_[name];

	LoadTexture(name);

	return textures_[name];
}
