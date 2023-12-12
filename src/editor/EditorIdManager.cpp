#include "EditorIdManager.h"

#include "D3E/Debug.h"

D3E::EditorIdManager* D3E::EditorIdManager::instance_ = nullptr;

D3E::EditorIdManager::~EditorIdManager()
{
	delete instance_;
}

D3E::EditorIdManager* D3E::EditorIdManager::Get()
{
	if (!instance_)
	{
		instance_ = new EditorIdManager;
	}
	return instance_;
}

uint32_t D3E::EditorIdManager::RegisterUuid(const eastl::string& uuid)
{
	if (freeIds_.empty())
	{
		ids_.push_back(uuid);
		return ids_.size() - 1;
	}

	const auto id = freeIds_.front();
	freeIds_.pop();
	ids_[id] = uuid;
	return id;
}

void D3E::EditorIdManager::UnregisterUuid(const eastl::string& uuid)
{
	for (uint32_t i = 1; i < ids_.size(); ++i)
	{
		if (ids_[i] == uuid)
		{
			freeIds_.push(i);
			ids_[i] = "null";
			return;
		}
	}
	Debug::LogWarning("[EditorIdManager] Tried to unregister uuid that is not registered.");
}

D3E::EditorIdManager::EditorIdManager()
{
	ids_.push_back("null");
}

eastl::string D3E::EditorIdManager::GetUuid(uint32_t id)
{
	if (id > ids_.size())
	{
		return "null";
	}
	return ids_[id];
}
