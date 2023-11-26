#pragma once

#include "SimpleMath.h"

namespace D3E
{
	struct CsmConstBuffer
	{
		DirectX::SimpleMath::Matrix gViewProj[4];
		DirectX::SimpleMath::Vector4 gDistances;
	};
}
