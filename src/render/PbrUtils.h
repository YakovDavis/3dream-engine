#pragma once

#include "D3E/CommonHeader.h"
#include "nvrhi/nvrhi.h"

namespace D3E
{
	namespace PbrUtils
	{
		void Setup(nvrhi::IDevice* device, nvrhi::ICommandList* commandList);
	};
}
