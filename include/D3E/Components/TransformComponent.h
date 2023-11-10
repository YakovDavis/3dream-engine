//
// Created by Laptop on 09.11.2023.
//

#ifndef D3ENGINE_TRANSFORMCOMPONENT_H
#define D3ENGINE_TRANSFORMCOMPONENT_H

#include "EASTL/fixed_vector.h"

struct TransformComponent
{
	eastl::fixed_vector<float, 3, false> position_;
	eastl::fixed_vector<float, 4, false> rotation_;
	eastl::fixed_vector<float, 3, false> scale_;
};

#endif // D3ENGINE_TRANSFORMCOMPONENT_H
