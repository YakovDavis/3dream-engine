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
		return DirectX::XMMatrixPerspectiveFovLH(cameraComponent.fov, cameraComponent.aspectRatio,
		                                         cameraComponent.nearPlane, cameraComponent.farPlane);
#else
		return Matrix::CreatePerspectiveFieldOfView(cameraComponent.fov, cameraComponent.aspectRatio,
		                                            cameraComponent.nearPlane, cameraComponent.farPlane);
#endif // USE_LH_MATRICES
	}
}
