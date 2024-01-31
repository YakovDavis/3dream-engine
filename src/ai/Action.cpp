#include "D3E/ai/Action.h"

using namespace D3E;

bool D3E::operator==(const Action& lhs, const Action& rhs)
{
	return lhs.GetName() == rhs.GetName() && lhs.GetCost() == rhs.GetCost() &&
	       lhs.GetPreconditions() == rhs.GetPreconditions() &&
	       lhs.GetEffects() == rhs.GetEffects();
}

bool D3E::operator!=(const Action& lhs, const Action& rhs)
{
	return !(lhs == rhs);
}