#pragma once

#include "../Common.h"
#include "AnonymousComponentContainer.h"

#include <map>
#include <string>

struct EntityArchetype final
{
	ULONG Hash;

	std::map<std::string, AnonymousComponentContainer> Components;

	EntityArchetype();

	~EntityArchetype() = default;
};
