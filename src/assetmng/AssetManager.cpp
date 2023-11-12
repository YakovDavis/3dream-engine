#include "D3E/AssetManager.h"

D3E::AssetManager D3E::AssetManager::instance_ = {};

D3E::AssetManager& D3E::AssetManager::Get()
{
	return instance_;
}
