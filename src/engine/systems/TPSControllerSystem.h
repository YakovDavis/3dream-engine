#pragma once

#include "D3E/CommonHeader.h"
#include "D3E/systems/GameSystem.h"
#include "D3E/Components/TPSControllerComponent.h"
#include "D3E/Components/TransformComponent.h"
#include "D3E/Components/render/CameraComponent.h"

namespace D3E
{
	class TPSControllerSystem : public GameSystem
	{
	public:
		void Update(entt::registry& reg, Game* game, float dT) override;

	private:
		void UpdateTpsComponent(TPSControllerComponent& tpscc, const CameraComponent& cc, Game* game);
		void UpdateCameraComponent(CameraComponent& cc, TransformComponent& tc, TPSControllerComponent& tpscc);
	};
}
