#pragma once

#include "entt/entt.hpp"
//#include "nvrhi/nvrhi.h"

namespace nvrhi
{
	class ICommandList;
	class IDevice;
	class IFramebuffer;
}

namespace D3E
{
	class Game;

	class GameSystem
	{
	public:
		virtual void Init() {}
		virtual void InitRender(nvrhi::ICommandList* commandList, nvrhi::IDevice* device) {}
		virtual void PreDraw(entt::registry& reg, nvrhi::ICommandList* commandList, nvrhi::IDevice* device) {}
		virtual void Draw(entt::registry& reg, nvrhi::IFramebuffer* fb, nvrhi::ICommandList* commandList, nvrhi::IDevice* device) {}
		virtual void PostDraw(entt::registry& reg, nvrhi::ICommandList* commandList, nvrhi::IDevice* device) {}
		virtual void PrePhysicsUpdate(entt::registry& reg, Game* game, float dT) {}
		virtual void PostPhysicsUpdate(entt::registry& reg) {}
		virtual void Update(entt::registry& reg, Game* game, float dT) {}
		virtual void RenderOnce(entt::registry& reg, nvrhi::IFramebuffer* fb, nvrhi::ICommandList* commandList, nvrhi::IDevice* device) {}
		virtual void RunOnce(entt::registry& reg, Game* game, float dT) {}
		virtual void Play(entt::registry& reg, Game* game) {}
		virtual void Stop(entt::registry& reg, Game* game) {}
	};
}
