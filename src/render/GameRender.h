#pragma once

#include "D3E/CommonHeader.h"

#include "GBuffer.h"
#include "D3E/systems/GameSystem.h"
#include "Display.h"
#include "EASTL/shared_ptr.h"
#include "EASTL/vector.h"
#include "NvrhiMessageCallback.h"
#include "nvrhi/nvrhi.h"

#include <Windows.h>

#define USE_IMGUI

#ifdef USE_IMGUI
#include "LightPass.h"
#include "editor/Editor.h"
#endif // USE_IMGUI

namespace D3E
{
	class Game;

	class DisplayWin32;

	class DebugRenderer;

	// Internal class for managing render devices, swap chains etc.
	class GameRender
	{
	public:
		virtual void Init(eastl::vector<GameSystem*>& systems);
		virtual void OnResize();

		virtual void PrimitiveBatchStart() {}

		Display* GetDisplay();
		DebugRenderer* GetDebugRenderer();
		nvrhi::DeviceHandle& GetDevice();
		nvrhi::CommandListHandle& GetCommandList();

		void CalculateFrameStats();

		uint32_t EditorPick(int x, int y);

		virtual void UpdateAnimations(float dT);

		virtual void PrepareDraw(entt::registry& registry, eastl::vector<GameSystem*>& systems, eastl::vector<GameSystem*>& renderPPSystems);
		virtual void Draw(entt::registry& registry, eastl::vector<GameSystem*>& systems, eastl::vector<GameSystem*>& renderPPSystems);
		virtual void EndDraw(entt::registry& registry, eastl::vector<GameSystem*>& systems, eastl::vector<GameSystem*>& renderPPSystems);

		virtual void Present() = 0;
		virtual UINT GetCurrentFrameBuffer() = 0;

//		void LoadTexture(const String& name, const String& fileName);

		void DestroyResources();

		explicit GameRender(Game* parent, HINSTANCE hInstance);
		virtual ~GameRender() = default;

	protected:
		Game* parentGame;

		nvrhi::DeviceHandle device_;

		nvrhi::CommandListHandle commandList_;

		eastl::shared_ptr<Display> display_;

		eastl::vector<nvrhi::TextureHandle> nvrhiSwapChain;

		nvrhi::TextureHandle nvrhiDepthBuffer;

		eastl::vector<nvrhi::FramebufferHandle> nvrhiFramebuffer;

		nvrhi::FramebufferHandle frameGBuffer;

		NvrhiMessageCallback* messageCallback_;

		nvrhi::InputLayoutHandle inputLayout_;

		D3E::Editor* editor_;

		GBuffer gbuffer_;

		nvrhi::BufferHandle pickedIdBuffer_;
		nvrhi::BufferHandle pickedIdBufferCpu_;

		nvrhi::BufferHandle pickCb;

		DebugRenderer* debugRenderer_;

		friend class Game;
	};
}

