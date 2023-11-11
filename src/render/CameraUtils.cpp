#include "CameraUtils.h"

using namespace DirectX::SimpleMath;

Matrix D3E::CameraUtils::GetViewProj(const eastl::fixed_vector<float, 3, false>& parentLoc, const D3E::CameraComponent& cameraComponent)
{
	return GetView(parentLoc, cameraComponent) * GetProj(cameraComponent);
}

Matrix D3E::CameraUtils::GetView(const eastl::fixed_vector<float, 3, false>& parentLoc, const D3E::CameraComponent& cameraComponent)
{
	Vector3 worldPos(parentLoc[0] + cameraComponent.offset[0], parentLoc[1] + cameraComponent.offset[1], parentLoc[2] + cameraComponent.offset[2]);
	Vector3 target(worldPos.x + cameraComponent.forward[0], worldPos.y + cameraComponent.forward[1], worldPos.z + cameraComponent.forward[2]);
	Vector3 up(cameraComponent.up[0], cameraComponent.up[1], cameraComponent.up[2]);
	return Matrix::CreateLookAt(worldPos, target, up);
}

Matrix D3E::CameraUtils::GetProj(const D3E::CameraComponent& cameraComponent)
{
	if (cameraComponent.isOrthographic)
	{
		return Matrix::CreateOrthographic(cameraComponent.orthographicWidth, cameraComponent.orthographicHeight,
		                                  cameraComponent.nearPlane / 10.0f, cameraComponent.farPlane);
	}
	else
	{
		return Matrix::CreatePerspectiveFieldOfView(cameraComponent.fov, cameraComponent.aspectRatio,
		                                            cameraComponent.nearPlane, cameraComponent.farPlane);
	}
}
