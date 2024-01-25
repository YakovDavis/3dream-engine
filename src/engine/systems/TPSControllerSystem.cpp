#include "TPSControllerSystem.h"

#include "D3E/CommonCpp.h"
#include "D3E/Debug.h"
#include "D3E/Game.h"
#include "EASTL/algorithm.h"
#include "SimpleMath.h"
#include "core/EngineState.h"
#include "input/InputDevice.h"
#include "input/Keys.h"
#include "sound_engine/SoundEngine.h"

#include <format>
#include <iostream>

#define XM_2PI 6.28318530718f

using namespace DirectX::SimpleMath;
using namespace D3E;

void D3E::TPSControllerSystem::Update(entt::registry& reg, D3E::Game* game,
                                      float dT)
{
	if (EngineState::currentPlayer == entt::null)
	{
		return;
	}
	TransformComponent* playerTransform = reg.try_get<TransformComponent>(EngineState::currentPlayer);
	if (!playerTransform)
	{
		return;
	}
	CameraComponent* camera = reg.try_get<CameraComponent>(EngineState::currentPlayer);
	if (!camera)
	{
		return;
	}
	TPSControllerComponent* tpsController = reg.try_get<TPSControllerComponent>(EngineState::currentPlayer);
	if (!tpsController)
	{
		return;
	}

	reg.patch<TPSControllerComponent>(
		EngineState::currentPlayer,
		[&, game, camera](auto& fpscc_) { UpdateTpsComponent(fpscc_, *camera, game); });

	reg.patch<CameraComponent>(EngineState::currentPlayer, [&, playerTransform](auto& cc_)
	                           { UpdateCameraComponent(cc_, *playerTransform, *tpsController); });
}

void TPSControllerSystem::UpdateTpsComponent(TPSControllerComponent& tpscc, const CameraComponent& cc,
                                             Game* game)
{
	if ((game->GetInputDevice()->IsKeyDown(Keys::RightButton) || !tpscc.isRMBActivated))
	{
		tpscc.phi += (tpscc.invertXAxis ? 1.0f : -1.0f) *
			tpscc.sensitivityX * game->GetInputDevice()->MouseOffsetInTick.x;
		while (tpscc.phi < -XM_2PI)
			tpscc.phi += XM_2PI;
		while (tpscc.phi > XM_2PI)
			tpscc.phi -= XM_2PI;
		tpscc.theta -= (tpscc.invertYAxis ? 1.0f : -1.0f) *
			tpscc.sensitivityY * game->GetInputDevice()->MouseOffsetInTick.y;
		while (tpscc.theta < -XM_2PI)
			tpscc.theta += XM_2PI;
		while (tpscc.theta > XM_2PI)
			tpscc.theta -= XM_2PI;

		if (tpscc.limitTheta)
		{
			tpscc.theta = eastl::clamp(tpscc.theta, tpscc.lowerThetaLimit, tpscc.upperThetaLimit);
		}
	}
}

void TPSControllerSystem::UpdateCameraComponent(CameraComponent& cc, TransformComponent& tc,
                                                TPSControllerComponent& tpscc)
{
	cc.up = Vector3::Transform(Vector3::Up, Matrix::CreateFromYawPitchRoll(XM_2PI/4.0f - tpscc.phi, -tpscc.theta, 0.0f));
	cc.forward = Vector3::Transform(Vector3::Forward, Matrix::CreateFromYawPitchRoll(XM_2PI/4.0f - tpscc.phi, -tpscc.theta, 0.0f));

	cc.offset = -5.0f * cc.forward;
}
