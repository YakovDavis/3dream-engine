#include "D3E/AssetManager.h"

#include "D3E/CommonCpp.h"

D3E::AssetManager D3E::AssetManager::instance_ = {};

D3E::AssetManager& D3E::AssetManager::Get()
{
	return instance_;
}
