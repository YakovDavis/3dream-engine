#pragma once

#include "EASTL/fixed_vector.h"

struct TransformComponent
{
	eastl::fixed_vector<float, 3, false> position_;
	eastl::fixed_vector<float, 4, false> rotation_;
	eastl::fixed_vector<float, 3, false> scale_;
};
