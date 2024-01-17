#pragma once

#include <DXGI.h>
#include "GameRender.h"
#include <dxgi1_5.h>
#include "D3dUtil.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

namespace D3E
{
	class GameRenderD3D12 final : public GameRender
	{
	public:
		explicit GameRenderD3D12(Game* parent, HINSTANCE hInstance);
		~GameRenderD3D12() override = default;

		void Init(eastl::vector<GameSystem*>& systems) override;
		void OnResize() override;
		void PrepareFrame() override;

		void Present() override;

		UINT GetCurrentFrameBuffer() override;

	protected:
		void InitD3D();

		void CreateCommandQueues();
		void CreateNativeSwapChain();
		void CreateNvrhiSwapChain();

		void FlushCommandQueue();

		[[nodiscard]] ID3D12Resource* CurrentBackBuffer() const;

		void LogAdapters();
		void LogAdapterOutputs(IDXGIAdapter* adapter);
		void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

		void UpdateDisplayWin32();

	protected:
		DXGI_SWAP_CHAIN_DESC1 mSwapChainDesc;
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC mFullScreenDesc;

		nvrhi::RefCountPtr<IDXGIFactory2> mdxgiFactory;
		nvrhi::RefCountPtr<IDXGISwapChain1> pSwapChain1;
		nvrhi::RefCountPtr<IDXGISwapChain3> mSwapChain;
		nvrhi::RefCountPtr<ID3D12Device> md3dDevice;
		nvrhi::RefCountPtr<IDXGIAdapter> mDxgiAdapter;

		nvrhi::RefCountPtr<ID3D12Fence> mFence;
		UINT64 mCurrentFence = 0;

		UINT64 mFrameCount = 1;

		nvrhi::RefCountPtr<ID3D12CommandQueue> mCommandQueue;
		nvrhi::RefCountPtr<ID3D12CommandQueue> mComputeCommandQueue;
		nvrhi::RefCountPtr<ID3D12CommandQueue> mCopyCommandQueue;

		eastl::vector<HANDLE> mFrameFenceEvents;

		int SwapChainBufferCount = 2;
		int mCurrBackBuffer = 0;
		eastl::vector<nvrhi::RefCountPtr<ID3D12Resource>> mSwapChainBuffer;

		D3D12_VIEWPORT mScreenViewport{};
		D3D12_RECT mScissorRect{};

		// Derived class should set these in derived constructor to customize starting values.
		std::string mMainWndCaption = "d3d App";
		D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
		DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		bool mTearingSupported = false;

	private:
		DisplayWin32* displayWin32_;
	};
}
