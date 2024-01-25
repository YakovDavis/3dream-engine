#pragma once

#include "D3E/CommonHeader.h"
#include "BaseComponent.h"
#include "SimpleMath.h"

namespace D3E
{
	struct TPSControllerComponent : public BaseComponent
	{
		float phi = 0.0f;
		float theta = 0.0f;
		float radius = 5.0f;
		float sensitivityX = 0.01f;
		float sensitivityY = 0.01f;
		bool isRMBActivated = true;
		bool limitTheta = true;
		float upperThetaLimit = DirectX::XM_PI / 2.0f;
		float lowerThetaLimit = 0.0f;
		bool invertXAxis = false;
		bool invertYAxis = false;

		void to_json(json& j) const override;
		void from_json(const json& j) override;
	};

	void to_json(json& j, const TPSControllerComponent& t);

	void from_json(const json& j, TPSControllerComponent& t);
}
