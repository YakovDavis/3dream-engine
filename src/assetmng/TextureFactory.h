#pragma once
#include "EASTL/string.h"
#include "EASTL/unordered_map.h"
#include "nvrhi/nvrhi.h"

#include <d3d12.h>

namespace D3E
{
	class Game;

	struct Texture
	{
		eastl::string Filename;
		nvrhi::TextureHandle Handle;
	};

	class TextureFactory
	{
	private:
		static bool isInitialized_;
		static Game* activeGame_;
		static eastl::unordered_map<eastl::string, Texture>	textures_;
		static eastl::unordered_map<eastl::string, nvrhi::SamplerHandle> samplers_;

	public:
		TextureFactory() = delete;
		static void Initialize(Game* game);
		static bool IsInitialized() { return isInitialized_; }
		static void DestroyResources();

		static nvrhi::TextureHandle GetTextureHandle(const eastl::string& name);
		static void LoadTexture(const eastl::string& name, const eastl::string& fileName, nvrhi::DeviceHandle& device, nvrhi::CommandListHandle commandList);

		static nvrhi::SamplerHandle& GetSampler(const eastl::string& name);
		static nvrhi::SamplerHandle& AddSampler(const eastl::string& name, nvrhi::IDevice* device, const nvrhi::SamplerDesc& desc);
	};
}