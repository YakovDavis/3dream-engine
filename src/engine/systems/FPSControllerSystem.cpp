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
				fpscc.yaw += fpscc.sensitivityX *
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
			Vector3 velocity = {0.f, 0.f, 0.f};

			if (game->GetInputDevice()->IsKeyDown(Keys::W))
			{
				Vector3 tmp = XMVector4Transform(
					Vector3(0, 0, 1), Matrix::CreateFromYawPitchRoll(
										  fpscc.yaw, fpscc.pitch, 0.0f));
				tmp.Normalize();
				tc.position_.x += dT * fpscc.speed * tmp.x;
				tc.position_.y += dT * fpscc.speed * tmp.y;
				tc.position_.z += dT * fpscc.speed * tmp.z;
			}
			if (game->GetInputDevice()->IsKeyDown(Keys::S))
			{
				Vector3 tmp = XMVector4Transform(
					Vector3(0, 0, -1), Matrix::CreateFromYawPitchRoll(
										   fpscc.yaw, fpscc.pitch, 0.0f));
				tmp.Normalize();
				tc.position_.x += dT * fpscc.speed * tmp.x;
				tc.position_.y += dT * fpscc.speed * tmp.y;
				tc.position_.z += dT * fpscc.speed * tmp.z;
			}
			if (game->GetInputDevice()->IsKeyDown(Keys::A))
			{
				Vector3 tmp = XMVector4Transform(
					Vector3(-1, 0, 0), Matrix::CreateFromYawPitchRoll(
										  fpscc.yaw, fpscc.pitch, 0.0f));
				tmp.Normalize();
				tc.position_.x += dT * fpscc.speed * tmp.x;
				tc.position_.y += dT * fpscc.speed * tmp.y;
				tc.position_.z += dT * fpscc.speed * tmp.z;
			}
			if (game->GetInputDevice()->IsKeyDown(Keys::D))
			{
				Vector3 tmp = XMVector4Transform(
					Vector3(1, 0, 0), Matrix::CreateFromYawPitchRoll(
										   fpscc.yaw, fpscc.pitch, 0.0f));
				tmp.Normalize();
				tc.position_.x += dT * fpscc.speed * tmp.x;
				tc.position_.y += dT * fpscc.speed * tmp.y;
				tc.position_.z += dT * fpscc.speed * tmp.z;
			}
			if (game->GetInputDevice()->IsKeyDown(Keys::E))
			{
				tc.position_.y += dT * fpscc.speed;
			}
			if (game->GetInputDevice()->IsKeyDown(Keys::Z))
			{
				tc.position_.y -= dT * fpscc.speed;
			}
			Vector4 Up = XMVector4Transform(
				Vector3(0, 1, 0),
				Matrix::CreateFromYawPitchRoll(fpscc.yaw, fpscc.pitch, 0.0f));
			Vector4 Forward = XMVector4Transform(
				Vector3(0, 0, 1),
				Matrix::CreateFromYawPitchRoll(fpscc.yaw, fpscc.pitch, 0.0f));
			cc.up.x = Up.x;
			cc.up.y = Up.y;
			cc.up.z = Up.z;
			cc.forward.x = Forward.x;
			cc.forward.y = Forward.y;
			cc.forward.z = Forward.z;

			velocity.x = (tc.position_.x - lastpos.x) * (1000 / dT);
			velocity.y = (tc.position_.y - lastpos.y) * (1000 / dT);
			velocity.z = (tc.position_.z - lastpos.z) * (1000 / dT);

			se->SetListenerTransform(tc.position_, velocity, cc.forward, cc.up);
		});
}
