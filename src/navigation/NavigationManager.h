#pragma once

#include "D3E/Game.h"
#include "SimpleMath.h"

namespace D3E
{
	class NavigationManager
	{
	public:
		NavigationManager(const NavigationManager&) = delete;
		NavigationManager& operator=(const NavigationManager&) = delete;

		static NavigationManager& GetInstance();

		static void MoveTo(entt::entity e,
		                   const DirectX::SimpleMath::Vector3& pos);
		static void CancelTarget(entt::entity e);

		void Init(Game* g);

	private:
		NavigationManager();

		void MoveToInternal(entt::entity e,
		                    const DirectX::SimpleMath::Vector3& pos);

		void CancelTargetInternal(entt::entity e);

		Game* game_;
	};
} // namespace D3E