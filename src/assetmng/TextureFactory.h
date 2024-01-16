#pragma once

#include "D3E/CommonHeader.h"
#include "EASTL/unordered_map.h"
#include "Texture2DMetaData.h"
#include "nvrhi/nvrhi.h"
#include "render/GBuffer.h"

#include <d3d12.h>

namespace D3E
{
	class Game;

	struct Texture
	{
		nvrhi::TextureHandle Handle;
		Texture2DMetaData MetaData;
	};

	class TextureFactory
	{
	private:
		static bool isInitialized_;
		static Game* activeGame_;
		static eastl::unordered_map<String, Texture> textures_;
		static eastl::unordered_map<String, nvrhi::SamplerHandle> samplers_;
		static GBuffer* gbuffer_;

	public:
		TextureFactory() = delete;
		static void Initialize(Game* game);
		static bool IsInitialized() { return isInitialized_; }
		static void DestroyResources();

		static void RegisterGBuffer(GBuffer* gbuffer) { gbuffer_ = gbuffer; }
		static GBuffer* GetGBuffer() { return gbuffer_; }

		static bool IsTextureUuidValid(const D3E::String& uuid);

		static nvrhi::TextureHandle GetTextureHandle(const String& uuid);
		static void LoadTexture(Texture2DMetaData& metaData, const std::string& directory, bool firstLoad, nvrhi::IDevice* device, nvrhi::ICommandList* commandList);
		static void UnloadTexture(const String& uuid);
		static void RenameTexture(const String& uuid, const String& name);

		static nvrhi::TextureHandle GetNewTextureHandle(const String& uuid);

		static nvrhi::SamplerHandle& GetSampler(const String& name);
		static nvrhi::SamplerHandle& AddSampler(const String& name, nvrhi::IDevice* device, const nvrhi::SamplerDesc& desc);
	};
}