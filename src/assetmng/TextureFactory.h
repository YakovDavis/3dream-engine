#pragma once

#include "D3E/CommonHeader.h"
#include "EASTL/unordered_map.h"
#include "nvrhi/nvrhi.h"

#include <d3d12.h>

namespace D3E
{
	class Game;

	struct Texture
	{
		String Filename;
		nvrhi::TextureHandle Handle;
	};

	class TextureFactory
	{
	private:
		static bool isInitialized_;
		static Game* activeGame_;
		static eastl::unordered_map<String, Texture> textures_;
		static eastl::unordered_map<String, nvrhi::SamplerHandle> samplers_;

	public:
		TextureFactory() = delete;
		static void Initialize(Game* game);
		static bool IsInitialized() { return isInitialized_; }
		static void DestroyResources();

		static nvrhi::TextureHandle GetTextureHandle(const String& name);
		static void LoadTexture(const String& name, const String& fileName, nvrhi::DeviceHandle& device, nvrhi::CommandListHandle& commandList);

		static nvrhi::SamplerHandle& GetSampler(const String& name);
		static nvrhi::SamplerHandle& AddSampler(const String& name, nvrhi::IDevice* device, const nvrhi::SamplerDesc& desc);
	};
}