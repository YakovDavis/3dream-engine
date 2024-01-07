#pragma once

#include "D3E/CommonHeader.h"
#include "EASTL/map.h"
#include "EASTL/vector.h"
#include "entt/entt.hpp"

namespace D3E
{
	class Game;

	class ComponentFactory
	{
	private:
		static bool isInitialized_;
		static Game* game_;
		static eastl::map<entt::id_type, String> typeNames_;

	public:
		ComponentFactory() = delete;
		static void Initialize(Game* game);
		static entt::entity ResolveEntity(const json& j);
		static void SerializeEntity(const entt::entity& e, json& j, bool recordUuid);
		static eastl::vector<String> GetAllEntityComponents(const entt::entity& e);
		static bool IsInitialized() { return isInitialized_; }
		static void DestroyResources();
	};
}
