#include "MaterialFactory.h"

#include "D3E/Debug.h"

bool D3E::MaterialFactory::isInitialized_ = false;
D3E::Game* D3E::MaterialFactory::game_;
eastl::unordered_map<D3E::String, D3E::Material> D3E::MaterialFactory::materials_ {};

void D3E::MaterialFactory::Initialize(D3E::Game* game)
{
	game_ = game;
	isInitialized_ = true;
}

void D3E::MaterialFactory::AddMaterial(const D3E::Material& m)
{
	if (materials_.find(m.uuid) == materials_.end())
	{
		materials_.insert({m.uuid, m});
	}
	else
	{
		materials_[m.uuid] = m;
	}
}

const D3E::Material& D3E::MaterialFactory::GetMaterial(const D3E::String& uuid)
{
	if (materials_.find(uuid) == materials_.end())
	{
		Debug::LogError("[MaterialFactory] Material not found: " + uuid);
	}

	return materials_[uuid];
}

bool D3E::MaterialFactory::IsMaterialUuidValid(const D3E::String& uuid)
{
	return materials_.find(uuid) != materials_.end();
}

void D3E::MaterialFactory::RemoveMaterial(const D3E::String& uuid)
{
	if (materials_.find(uuid) == materials_.end())
	{
		return;
	}

	materials_.erase(uuid);
}

void D3E::MaterialFactory::RenameMaterial(const D3E::String& uuid,
                                          const D3E::String& name)
{
	if (materials_.find(uuid) == materials_.end())
	{
		return;
	}

	materials_[uuid].name = name;
}
