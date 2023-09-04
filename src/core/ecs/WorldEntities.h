#pragma once

#include <map>

struct EntityArchetype;
struct WorldEntities final
{
	std::map<long, EntityArchetype> Archetypes;

	WorldEntities();

	~WorldEntities() = default;
};
