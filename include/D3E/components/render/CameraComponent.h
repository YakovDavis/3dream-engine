#pragma once

#include "D3E/CommonHeader.h"
#include "D3E/Components/BaseComponent.h"
#include "SimpleMath.h"

namespace D3E
{
	struct CameraComponent : public BaseComponent
	{
		DirectX::SimpleMath::Vector3 offset = {0, 0, 0};
		DirectX::SimpleMath::Vector3 initialOffset = {0, 0, 0};
		DirectX::SimpleMath::Vector3 forward = {0, 0, 1};
		DirectX::SimpleMath::Vector3 up = {0, 1, 0};
		bool isOrthographic = false;
		float orthographicWidth = 50.0f;
		float orthographicHeight = 50.0f;
		float fov = 0.785f;
		float aspectRatio = 1280.0f / 720.0f;
		float nearPlane = 1.0f;
		float farPlane = 1000.f;

		DirectX::BoundingFrustum boundingFrustum;

		CameraComponent();

		void to_json(json& j) const override;
		void from_json(const json& j) override;
	};

	void to_json(json& j, const CameraComponent& t);

	void from_json(const json& j, CameraComponent& t);
}
