#pragma once

#include "D3E/components/render/CameraComponent.h"
#include "SimpleMath.h"

namespace D3E
{
	class CameraUtils
	{
	public:
		static DirectX::SimpleMath::Matrix GetViewProj(const eastl::fixed_vector<float, 3, false>& parentLoc, const CameraComponent& cameraComponent);
		static DirectX::SimpleMath::Matrix GetView(const eastl::fixed_vector<float, 3, false>& parentLoc, const CameraComponent& cameraComponent);
		static DirectX::SimpleMath::Matrix GetProj(const CameraComponent& cameraComponent);
	};
}
