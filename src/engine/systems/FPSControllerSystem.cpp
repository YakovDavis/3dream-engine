#include "FPSControllerSystem.h"

#include "D3E/Components/FPSControllerComponent.h"
#include "D3E/Components/TransformComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Debug.h"
#include "D3E/Game.h"
#include "SimpleMath.h"
#include "input/InputDevice.h"
#include "input/Keys.h"
#include "sound_engine/SoundEngine.h"

#include <iostream>

#define XM_2PI 6.28318530718f

using namespace DirectX::SimpleMath;

void D3E::FPSControllerSystem::Run(entt::registry& reg, Game* game, float dT)
{
	auto view =
		reg.view<TransformComponent, CameraComponent, FPSControllerComponent>();

	SoundEngine* se = &SoundEngine::GetInstance();

	view.each(
		[game, dT, se](auto& tc, auto& cc, auto& fpscc)
		{
			if (game->GetInputDevice()->IsKeyDown(Keys::LeftButton) ||
		        !fpscc.isLMBActivated)
			{
				fpscc.yaw -= fpscc.sensitivityX *
			                 game->GetInputDevice()->MouseOffsetInTick.x;
				while (fpscc.yaw < -XM_2PI)
					fpscc.yaw += XM_2PI;
				while (fpscc.yaw > XM_2PI)
					fpscc.yaw -= XM_2PI;
				fpscc.pitch += fpscc.sensitivityY *
			                   game->GetInputDevice()->MouseOffsetInTick.y;
				while (fpscc.pitch < -XM_2PI)
					fpscc.pitch += XM_2PI;
				while (fpscc.pitch > XM_2PI)
					fpscc.pitch -= XM_2PI;
			}

			const auto lastpos = tc.position_;
			eastl::fixed_vector<float, 3, false> velocity = {0.f, 0.f, 0.f};

			if (game->GetInputDevice()->IsKeyDown(Keys::W))
			{
				Vector3 tmp = XMVector4Transform(
					Vector3(0, 0, 1), Matrix::CreateFromYawPitchRoll(
										  fpscc.yaw, fpscc.pitch, 0.0f));
				tmp.Normalize();
				tc.position_[0] += dT * fpscc.speed * tmp.x;
				tc.position_[1] += dT * fpscc.speed * tmp.y;
				tc.position_[2] += dT * fpscc.speed * tmp.z;
			}
			if (game->GetInputDevice()->IsKeyDown(Keys::S))
			{
				Vector3 tmp = XMVector4Transform(
					Vector3(0, 0, -1), Matrix::CreateFromYawPitchRoll(
										   fpscc.yaw, fpscc.pitch, 0.0f));
				tmp.Normalize();
				tc.position_[0] += dT * fpscc.speed * tmp.x;
				tc.position_[1] += dT * fpscc.speed * tmp.y;
				tc.position_[2] += dT * fpscc.speed * tmp.z;
			}
			if (game->GetInputDevice()->IsKeyDown(Keys::A))
			{
				Vector3 tmp = XMVector4Transform(
					Vector3(1, 0, 0), Matrix::CreateFromYawPitchRoll(
										  fpscc.yaw, fpscc.pitch, 0.0f));
				tmp.Normalize();
				tc.position_[0] += dT * fpscc.speed * tmp.x;
				tc.position_[1] += dT * fpscc.speed * tmp.y;
				tc.position_[2] += dT * fpscc.speed * tmp.z;
			}
			if (game->GetInputDevice()->IsKeyDown(Keys::D))
			{
				Vector3 tmp = XMVector4Transform(
					Vector3(-1, 0, 0), Matrix::CreateFromYawPitchRoll(
										   fpscc.yaw, fpscc.pitch, 0.0f));
				tmp.Normalize();
				tc.position_[0] += dT * fpscc.speed * tmp.x;
				tc.position_[1] += dT * fpscc.speed * tmp.y;
				tc.position_[2] += dT * fpscc.speed * tmp.z;
			}
			if (game->GetInputDevice()->IsKeyDown(Keys::E))
			{
				tc.position_[1] += dT * fpscc.speed;
			}
			if (game->GetInputDevice()->IsKeyDown(Keys::Z))
			{
				tc.position_[1] -= dT * fpscc.speed;
			}
			Vector4 Up = XMVector4Transform(
				Vector3(0, 1, 0),
				Matrix::CreateFromYawPitchRoll(fpscc.yaw, fpscc.pitch, 0.0f));
			Vector4 Forward = XMVector4Transform(
				Vector3(0, 0, 1),
				Matrix::CreateFromYawPitchRoll(fpscc.yaw, fpscc.pitch, 0.0f));
			cc.up[0] = Up.x;
			cc.up[1] = Up.y;
			cc.up[2] = Up.z;
			cc.forward[0] = Forward.x;
			cc.forward[1] = Forward.y;
			cc.forward[2] = Forward.z;

			velocity[0] = (tc.position_[0] - lastpos[0]) * (1000 / dT);
			velocity[1] = (tc.position_[1] - lastpos[1]) * (1000 / dT);
			velocity[2] = (tc.position_[2] - lastpos[2]) * (1000 / dT);

			se->SetListenerTransform(tc.position_, velocity, cc.forward, cc.up);
		});
}
