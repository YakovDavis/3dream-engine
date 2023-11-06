#pragma once
#include "EASTL/unordered_map.h"
#include "EASTL/string.h"
#include "nvrhi/nvrhi.h"

namespace D3E
{
	class Game;

	class TextureFactory
	{
	private:
		static bool isInitialized_;
		static Game* activeGame_;
		static eastl::unordered_map<eastl::string, nvrhi::TextureHandle>
			textures_;
		static void LoadTexture(const eastl::string& name);

	public:
		TextureFactory() = delete;
		static void Initialize(Game* game);
		static bool IsInitialized() { return isInitialized_; }
		static void DestroyResources();
		static nvrhi::TextureHandle GetTexture(const eastl::string& name);
	};
}