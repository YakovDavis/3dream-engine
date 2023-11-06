#include "GameRenderD3D12.h"

#include "D3E/Game.h"
#include "D3E/Debug.h"
#include "DisplayWin32.h"

#include <iostream>
#include <nvrhi/d3d12.h>
#include <nvrhi/validation.h>

#include "ShaderFactory.h"

void D3E::GameRenderD3D12::CreateCommandQueues()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 1;
	ThrowIfFailed(md3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));
}

void D3E::GameRenderD3D12::CreateNativeSwapChain()
{
	mSwapChain.Reset();

	assert(displayWin32_->hWnd != nullptr);

	Debug::LogMessage("[GameRenderD3D12] Creating native swapchain");

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = displayWin32_->ClientWidth;
	sd.BufferDesc.Height = displayWin32_->ClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = mBackBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount;
	sd.OutputWindow = displayWin32_->hWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hres = mdxgiFactory->CreateSwapChain(
		mCommandQueue.Get(),
		&sd,
		mSwapChain.GetAddressOf());

	if (FAILED(hres))
	{
		Debug::LogError("[GameRenderD3D12] Native swapchain creation failed");
		Debug::Assert(true, "Native swapchain creation failed");
	}

	Debug::LogMessage("[GameRenderD3D12] Native swapchain creation finished");
}

void D3E::GameRenderD3D12::Init()
{
	UpdateDisplayWin32();

	Debug::LogMessage("[GameRenderD3D12] Init started");

	InitD3D();

	Debug::LogMessage("[GameRenderD3D12] Init finished");

	//OnResize();

	ShaderFactory::Initialize(dynamic_cast<Game*>(parentApp));

	GameRender::Init();
}

void D3E::GameRenderD3D12::FlushCommandQueue()
{
	// Advance the fence value to mark commands up to this fence point.
	mCurrentFence++;

	// Add an instruction to the command queue to set a new fence point.  Because we
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	mCommandQueue->Signal(mFence.Get(), mCurrentFence);

	// Wait until the GPU has completed commands up to this fence point.
	if(mFence->GetCompletedValue() < mCurrentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

		// Fire event when GPU hits current fence.
		mFence->SetEventOnCompletion(mCurrentFence, eventHandle);

		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

void D3E::GameRenderD3D12::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));


	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
}

void D3E::GameRenderD3D12::InitD3D()
{
#if defined(DEBUG) || defined(_DEBUG)
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
		debugController->EnableDebugLayer();
	}
#endif

	CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory));

	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_12_0,
		IID_PPV_ARGS(&md3dDevice));

	if(FAILED(hardwareResult))
	{
		Debug::LogError("[GameRenderD3D12] Can't find any D3D12 capable device");
	}

	nvrhi::d3d12::DeviceDesc deviceDesc;
	deviceDesc.errorCB = messageCallback_;
	deviceDesc.pDevice = md3dDevice;
	deviceDesc.pGraphicsCommandQueue = mCommandQueue;
	device_ = nvrhi::d3d12::createDevice(deviceDesc);

	md3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE,
	                        IID_PPV_ARGS(&mFence));

	mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

#ifdef _DEBUG
	LogAdapters();
#endif

	CreateCommandQueues();
	CreateNativeSwapChain();
	CreateNvrhiSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
}

void D3E::GameRenderD3D12::OnResize()
{
	D3E::GameRender::OnResize();

	Debug::Assert(md3dDevice, "[GameRenderD3D12] OnResize failed - invalid device");
	Debug::Assert(mSwapChain, "[GameRenderD3D12] OnResize failed - invalid swap chain");
	assert(mSwapChain);

	// Flush before changing any resources.
	FlushCommandQueue();

	// Release the previous resources we will be recreating.
	for (auto & i : mSwapChainBuffer)
	{
		i.Reset();
	}
	mDepthStencilBuffer.Reset();

	// Resize the swap chain.
	ThrowIfFailed(mSwapChain->ResizeBuffers(
		SwapChainBufferCount,
		displayWin32_->ClientWidth, displayWin32_->ClientHeight,
		mBackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	mCurrBackBuffer = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])));
		md3dDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, mRtvDescriptorSize);
	}

	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = displayWin32_->ClientWidth;
	depthStencilDesc.Height = displayWin32_->ClientHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;

	// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from
	// the depth buffer.  Therefore, because we need to create two views to the same resource:
	//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
	//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
	// we need to create the depth buffer resource with a typeless format.
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = mDepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	auto defaultProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&defaultProperties,
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));

	// Create descriptor to mip level 0 of entire resource using the format of the resource.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = mDepthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	md3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, DepthStencilView());

	auto defaultTransition = CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);

	// Wait until resize is complete.
	FlushCommandQueue();

	// Update the viewport transform to cover the client area.
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(displayWin32_->ClientWidth);
	mScreenViewport.Height = static_cast<float>(displayWin32_->ClientHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	mScissorRect = {0, 0, displayWin32_->ClientWidth, displayWin32_->ClientHeight};
}

ID3D12Resource* D3E::GameRenderD3D12::CurrentBackBuffer() const
{
	return mSwapChainBuffer[mCurrBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE D3E::GameRenderD3D12::CurrentBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
		mCurrBackBuffer,
		mRtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE D3E::GameRenderD3D12::DepthStencilView() const
{
	return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void D3E::GameRenderD3D12::LogAdapters()
{
	UINT i = 0;
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*> adapterList;
	while(mdxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		std::wstring text = L"***Adapter: ";
		text += desc.Description;
		text += L"\n";

		OutputDebugString(reinterpret_cast<LPCSTR>(text.c_str()));

		adapterList.push_back(adapter);

		++i;
	}

	for(auto & i : adapterList)
	{
		LogAdapterOutputs(i);
		ReleaseCom(i);
	}
}

void D3E::GameRenderD3D12::LogAdapterOutputs(IDXGIAdapter* adapter)
{
	UINT i = 0;
	IDXGIOutput* output = nullptr;
	while(adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		std::wstring text = L"***Output: ";
		text += desc.DeviceName;
		text += L"\n";
		OutputDebugString(reinterpret_cast<LPCSTR>(text.c_str()));

		LogOutputDisplayModes(output, mBackBufferFormat);

		ReleaseCom(output);

		++i;
	}
}

void D3E::GameRenderD3D12::LogOutputDisplayModes(IDXGIOutput* output,
                                                 DXGI_FORMAT format)
{
	UINT count = 0;
	UINT flags = 0;

	// Call with nullptr to get list count.
	output->GetDisplayModeList(format, flags, &count, nullptr);

	std::vector<DXGI_MODE_DESC> modeList(count);
	output->GetDisplayModeList(format, flags, &count, &modeList[0]);

	for(auto& x : modeList)
	{
		UINT n = x.RefreshRate.Numerator;
		UINT d = x.RefreshRate.Denominator;
		std::wstring text =
			L"Width = " + std::to_wstring(x.Width) + L" " +
			L"Height = " + std::to_wstring(x.Height) + L" " +
			L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
			L"\n";

		::OutputDebugString(reinterpret_cast<LPCSTR>(text.c_str()));
	}
}

D3E::GameRenderD3D12::GameRenderD3D12(App* parent, HINSTANCE hInstance)
	: GameRender(parent, hInstance)
{
}

void D3E::GameRenderD3D12::UpdateDisplayWin32()
{
	displayWin32_ = dynamic_cast<DisplayWin32*>(display_.get());
}

void D3E::GameRenderD3D12::CreateNvrhiSwapChain()
{
	mSwapChainBuffer.resize(SwapChainBufferCount);
	nvrhiSwapChainBuffer.resize(SwapChainBufferCount);

	Debug::LogMessage("[GameRenderD3D12] NVRHI swapchain creation started");

	for (int i = 0; i < SwapChainBufferCount; i++)
	{
		HRESULT hres = mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i]));

		if (FAILED(hres))
		{
			Debug::LogError("[DisplayWin32] Failed to retrieve swapchain buffer");
		}

		nvrhi::TextureDesc textureDesc;
		textureDesc.format = nvrhi::Format::RGBA8_UNORM;
		textureDesc.width = displayWin32_->ClientWidth;
		textureDesc.height = displayWin32_->ClientHeight;
		textureDesc.isRenderTarget = true;
		textureDesc.debugName = "SwapChainBuffer";
		textureDesc.sampleCount = 1;
		textureDesc.sampleQuality = 0;
		textureDesc.isUAV = false;
		textureDesc.initialState = nvrhi::ResourceStates::Present;
		textureDesc.keepInitialState = true;

		nvrhiSwapChainBuffer[i] = device_->createHandleForNativeTexture(
			nvrhi::ObjectTypes::D3D12_Resource, nvrhi::Object(mSwapChainBuffer[i]),
			textureDesc);
	}

	Debug::LogMessage("[GameRenderD3D12] NVRHI swapchain creation finished");
}
