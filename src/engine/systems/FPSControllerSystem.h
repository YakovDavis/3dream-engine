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
		void UpdateFpsComponent(FPSControllerComponent& fpscc, Game* game);
		void UpdateTransformComponent(TransformComponent& tc,
		                              FPSControllerComponent& fpscc, Game* game,
		                              float dT);
		void UpdateCameraComponent(CameraComponent& cc,
		                           FPSControllerComponent& fpscc);
	};
} // namespace D3E
