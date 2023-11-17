#pragma once

#include "D3E/Components/FPSControllerComponent.h"
#include "D3E/Components/TransformComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/systems/PerTickSystem.h"

namespace D3E
{
	class FPSControllerSystem : public PerTickSystem
	{
	public:
		void Run(entt::registry& reg, Game* game, float dT) override;

	private:
		void UpdateCameraComponent(CameraComponent& cc);
		void UpdateTransformComponent(TransformComponent& tc);

		TransformComponent tc_;
		CameraComponent cc_;
		FPSControllerComponent fpscc_;
	};
} // namespace D3E
