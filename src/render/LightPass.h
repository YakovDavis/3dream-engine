#pragma once

#include "D3E/Components/render/LightComponent.h"
#include "nvrhi/nvrhi.h"

namespace D3E
{
	class LightPass
	{
	public:
		void Run(const LightComponent& lc, nvrhi::ICommandList* commandList);
	};
}
