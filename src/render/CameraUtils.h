#pragma once

#include "D3E/components/render/CameraComponent.h"
#include "EASTL/vector.h"
#include "SimpleMath.h"

namespace D3E
{
	class CameraUtils
	{
	public:
		static DirectX::SimpleMath::Matrix GetViewProj(const DirectX::SimpleMath::Vector3& parentLoc, const CameraComponent& cameraComponent);
		static DirectX::SimpleMath::Matrix GetView(const DirectX::SimpleMath::Vector3& parentLoc, const CameraComponent& cameraComponent);
		static DirectX::SimpleMath::Matrix GetProj(const CameraComponent& cameraComponent);
		static eastl::vector<DirectX::SimpleMath::Vector4> GetFrustumCornersWorldSpace(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj);
	};
}
