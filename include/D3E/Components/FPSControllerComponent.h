#pragma once

#include "BaseComponent.h"
#include "D3E/CommonHeader.h"

namespace D3E
{
	struct D3EAPI FPSControllerComponent : public BaseComponent
	{
		float yaw = 3.14f;
		float pitch = 0.0f;
		float speed = 0.02f;
		float sensitivityX = 0.01f;
		float sensitivityY = 0.01f;
		bool isLMBActivated = false;
		bool isRMBActivated = true;

		void to_json(json& j) const override;
		void from_json(const json& j) override;
	};

	void to_json(json& j, const FPSControllerComponent& t);

	void from_json(const json& j, FPSControllerComponent& t);
}
