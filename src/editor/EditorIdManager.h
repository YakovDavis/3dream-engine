#pragma once

#include "EASTL/string.h"
#include "EASTL/vector.h"
#include "EASTL/queue.h"

namespace D3E
{
	class EditorIdManager
	{
	private:
		static EditorIdManager* instance_;
		eastl::vector<eastl::string> ids_;
		eastl::queue<uint32_t> freeIds_;
		EditorIdManager();

	public:
		~EditorIdManager();
		static EditorIdManager* Get();
		uint32_t RegisterUuid(const eastl::string& uuid);
		void UnregisterUuid(const eastl::string& uuid);
		void UnregisterAll();
		eastl::string GetUuid(uint32_t id);
	};
}
