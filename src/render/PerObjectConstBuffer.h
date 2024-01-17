#pragma once

#include "SimpleMath.h"

namespace D3E
{
	struct PerObjectConstBuffer
	{
		DirectX::SimpleMath::Matrix gWorldViewProj;
		DirectX::SimpleMath::Matrix gWorld;
		DirectX::SimpleMath::Matrix gWorldView;
		DirectX::SimpleMath::Matrix gInvTrRotation;
		uint32_t gEditorId;
	};
}