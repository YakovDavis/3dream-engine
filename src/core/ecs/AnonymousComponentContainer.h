#pragma once

#include "ComponentContainer.h"

struct AnonymousComponentContainer
{
	void* Data;

	template<typename Component> ComponentContainer<Component>* Cast();
};

template<typename Component>
ComponentContainer<Component>* AnonymousComponentContainer::Cast()
{
	return reinterpret_cast<Component>(Data);
}
