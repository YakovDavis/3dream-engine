#include "CameraUtils.h"

#include "D3E/CommonCpp.h"

#define USE_LH_MATRICES

using namespace DirectX::SimpleMath;

Matrix D3E::CameraUtils::GetViewProj(const Vector3& parentLoc, const D3E::CameraComponent& cameraComponent)
{
	return GetView(parentLoc, cameraComponent) * GetProj(cameraComponent);
}

Matrix D3E::CameraUtils::GetView(const Vector3& parentLoc, const D3E::CameraComponent& cameraComponent)
{
	Vector3 worldPos(parentLoc.x + cameraComponent.offset.x, parentLoc.y + cameraComponent.offset.y, parentLoc.z + cameraComponent.offset.z);
	Vector3 target(worldPos.x + cameraComponent.forward.x, worldPos.y + cameraComponent.forward.y, worldPos.z + cameraComponent.forward.z);
	Vector3 up(cameraComponent.up.x, cameraComponent.up.y, cameraComponent.up.z);
#ifdef USE_LH_MATRICES
	// return DirectX::XMMatrixLookAtLH(Vector3(-0.02f, 0.0f, 10.5f), Vector3(-0.02f, 0.0f, 10.5f) + Vector3(0.71f, 0.71f, 0.0f), Vector3(0.71f, 0.71f, 0.0f));
	return DirectX::XMMatrixLookAtLH(worldPos, target, up);
#else
	return Matrix::CreateLookAt(worldPos, target, up);
#endif // USE_LH_MATRICES
}

Matrix D3E::CameraUtils::GetProj(const D3E::CameraComponent& cameraComponent)
{
	if (cameraComponent.isOrthographic)
	{
#ifdef USE_LH_MATRICES
		return DirectX::XMMatrixOrthographicLH(cameraComponent.orthographicWidth, cameraComponent.orthographicHeight,
		                                       cameraComponent.nearPlane / 10.0f, cameraComponent.farPlane);
#else
		return Matrix::CreateOrthographic(cameraComponent.orthographicWidth, cameraComponent.orthographicHeight,
		                                  cameraComponent.nearPlane / 10.0f, cameraComponent.farPlane);
#endif // USE_LH_MATRICES
	}
	else
	{
#ifdef USE_LH_MATRICES
		// return DirectX::XMMatrixOrthographicOffCenterLH(-9.52f, 16.25, -16.27, 16.25, -81.37, 81.26);
		return DirectX::XMMatrixPerspectiveFovLH(cameraComponent.fov, cameraComponent.aspectRatio,
		                                         cameraComponent.nearPlane, cameraComponent.farPlane);
#else
		return Matrix::CreatePerspectiveFieldOfView(cameraComponent.fov, cameraComponent.aspectRatio,
		                                            cameraComponent.nearPlane, cameraComponent.farPlane);
#endif // USE_LH_MATRICES
	}
}

eastl::vector<DirectX::SimpleMath::Vector4>
D3E::CameraUtils::GetFrustumCornersWorldSpace(
	const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	const auto viewProj = view * proj;
	const auto inv = viewProj.Invert();

	eastl::vector<Vector4> frustumCorners;
	frustumCorners.reserve(8);
	for (unsigned int x = 0; x < 2; ++x)
	{
		for (unsigned int y = 0; y < 2; ++y)
		{
			for (unsigned int z = 0; z < 2; ++z)
			{
				const Vector4 pt = Vector4::Transform(Vector4(2.0f * static_cast<float>(x)- 1.0f,
				                                              2.0f * static_cast<float>(y) - 1.0f,
				                                              static_cast<float>(z), 1.0f),
				                                      		  inv);
				frustumCorners.push_back(pt / pt.w);
			}
		}
	}

	return frustumCorners;
}
