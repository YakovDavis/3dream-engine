#pragma once

#include "D3E/CommonHeader.h"
#include "D3E/render/Material.h"
#include "EASTL/unordered_map.h"

namespace D3E
{
	class Game;

	class MaterialFactory
	{
	public:
		MaterialFactory() = delete;

		static void Initialize(Game* game);
		static void AddMaterial(const Material& m);
		static void RenameMaterial(const String& uuid, const String& name);
		static void RemoveMaterial(const String& uuid);
		static const Material& GetMaterial(const String& uuid);
		static bool IsMaterialUuidValid(const String& uuid);

	private:
		static bool isInitialized_;
		static Game* game_;
		static eastl::unordered_map<String, Material> materials_;
	};
}
