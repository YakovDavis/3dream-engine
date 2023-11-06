#include "GameRender.h"

#include "D3E/App.h"
#include "D3E/Common.h"
#include "D3E/Debug.h"
#include "DisplayWin32.h"
#include "ShaderFactory.h"
#include "Vertex.h"

void D3E::GameRender::Init()
{
	Debug::LogMessage("[GameRender] Init started");
	if (!device_.Get())
	{
		Debug::LogError("[GameRender] GAPI not initialized");
		Debug::Assert(true, "GAPI not initialized");
	}

	ShaderFactory::AddVertexShader("Bare", "Bare.hlsl", "VSMain");

	nvrhi::VertexAttributeDesc attributes[] = {
		nvrhi::VertexAttributeDesc()
			.setName("POSITION")
			.setFormat(nvrhi::Format::RGBA32_FLOAT)
			.setOffset(offsetof(Vertex, pos))
			.setElementStride(sizeof(Vertex)),
		nvrhi::VertexAttributeDesc()
			.setName("TEXCOORD")
			.setFormat(nvrhi::Format::RGBA32_FLOAT)
			.setOffset(offsetof(Vertex, tex))
			.setElementStride(sizeof(Vertex)),
		nvrhi::VertexAttributeDesc()
			.setName("NORMAL")
			.setFormat(nvrhi::Format::RGBA32_FLOAT)
			.setOffset(offsetof(Vertex, normal))
			.setElementStride(sizeof(Vertex)),
	};
	inputLayout_ = device_->createInputLayout(attributes,
	                                          uint32_t(std::size(attributes)),
	                                          ShaderFactory::GetVertexShader("Base3d"));

	ShaderFactory::AddPixelShader("Bare", "Bare.hlsl", "PSMain");

	nvrhi::FramebufferDesc framebufferDesc0 = {};
	framebufferDesc0.addColorAttachment(nvrhiSwapChainBuffer[0]);
	nvrhi::FramebufferHandle framebuffer0 = device_->createFramebuffer(framebufferDesc0);

	nvrhi::FramebufferDesc framebufferDesc1 = {};
	framebufferDesc1.addColorAttachment(nvrhiSwapChainBuffer[1]);
	nvrhi::FramebufferHandle framebuffer1 = device_->createFramebuffer(framebufferDesc1);

	auto layoutDesc = nvrhi::BindingLayoutDesc()
	                      .setVisibility(nvrhi::ShaderType::All);
	                      //.addItem(nvrhi::BindingLayoutItem::Texture_SRV(0))
	                      //.addItem(nvrhi::BindingLayoutItem::VolatileConstantBuffer(0));
	nvrhi::BindingLayoutHandle bindingLayout = device_->createBindingLayout(layoutDesc);

	auto pipelineDesc = nvrhi::GraphicsPipelineDesc()
	                        .setInputLayout(inputLayout_)
	                        .setVertexShader(ShaderFactory::GetVertexShader("Bare"))
	                        .setPixelShader(ShaderFactory::GetPixelShader("Bare"))
	                        .addBindingLayout(bindingLayout);
	nvrhi::GraphicsPipelineHandle graphicsPipeline = device_->createGraphicsPipeline(pipelineDesc, framebuffer0);
	Debug::LogMessage("[GameRender] Init finished");
}

void D3E::GameRender::DestroyResources()
{
}

void D3E::GameRender::OnResize()
{
}

void D3E::GameRender::CalculateFrameStats()
{
	/* TODO: recreate using our timers
	// Code computes the average frames per second, and also the
	// average time it takes to render one frame.  These stats
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if((mTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wstring fpsStr = std::to_wstring(fps);
		std::wstring mspfStr = std::to_wstring(mspf);

		std::wstring windowText = mMainWndCaption +
		                     L"    fps: " + fpsStr +
		                     L"   mspf: " + mspfStr;

		SetWindowText(mhMainWnd, windowText.c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
	*/
}

D3E::GameRender::GameRender(App* parent, HINSTANCE hInstance) : parentApp(parent)
{
	assert(parent != nullptr);
	assert(hInstance != nullptr);
	display_ = eastl::make_shared<DisplayWin32>(reinterpret_cast<LPCWSTR>("john cena"), hInstance, 640, 480, parent);
	messageCallback_ = new NvrhiMessageCallback();
}

D3E::Display* D3E::GameRender::GetDisplay()
{
	return display_.get();
}

nvrhi::DeviceHandle& D3E::GameRender::GetDevice()
{
	return device_;
}

nvrhi::CommandListHandle& D3E::GameRender::GetCommandList()
{
	return commandList_;
}
