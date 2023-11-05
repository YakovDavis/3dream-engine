#pragma once

#include <string>
#include "GameRender.h"
#include <dxgi1_5.h>
#include "D3dUtil.h"
#include "eastl/vector.h"

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

namespace D3E
{
	class GameRenderD3D12 final : public GameRender
	{
	public:
		explicit GameRenderD3D12(App* parent, HINSTANCE hInstance);
		~GameRenderD3D12() override = default;

		void Init() override;
		void OnResize() override;

	protected:
		void InitD3D();

		void CreateCommandQueues();
		void CreateNativeSwapChain();
		void CreateNvrhiSwapChain();

		void FlushCommandQueue();

		[[nodiscard]] ID3D12Resource* CurrentBackBuffer() const;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;
		virtual void CreateRtvAndDsvDescriptorHeaps();

		void LogAdapters();
		void LogAdapterOutputs(IDXGIAdapter* adapter);
		void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

		void UpdateDisplayWin32();

	protected:
		nvrhi::RefCountPtr<IDXGIFactory4> mdxgiFactory;
		nvrhi::RefCountPtr<IDXGISwapChain> mSwapChain;
		nvrhi::RefCountPtr<ID3D12Device> md3dDevice;

		nvrhi::RefCountPtr<ID3D12Fence> mFence;
		UINT64 mCurrentFence = 0;

		nvrhi::RefCountPtr<ID3D12CommandQueue> mCommandQueue;

		int SwapChainBufferCount = 2;
		int mCurrBackBuffer = 0;
		eastl::vector<nvrhi::RefCountPtr<ID3D12Resource>> mSwapChainBuffer;
		eastl::vector<nvrhi::TextureHandle> nvrhiSwapChainBuffer;
		nvrhi::RefCountPtr<ID3D12Resource> mDepthStencilBuffer;

		nvrhi::RefCountPtr<ID3D12DescriptorHeap> mRtvHeap;
		nvrhi::RefCountPtr<ID3D12DescriptorHeap> mDsvHeap;

		D3D12_VIEWPORT mScreenViewport{};
		D3D12_RECT mScissorRect{};

		UINT mRtvDescriptorSize = 0;
		UINT mDsvDescriptorSize = 0;
		UINT mCbvSrvUavDescriptorSize = 0;

		// Derived class should set these in derived constructor to customize starting values.
		std::string mMainWndCaption = "d3d App";
		D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
		DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	private:
		DisplayWin32* displayWin32_;
	};
}
