#include "GameRender.h"

#include "D3E/CommonCpp.h"
#include "CameraUtils.h"
#include "D3E/CommonHeader.h"
#include "D3E/Debug.h"
#include "D3E/Game.h"
#include "D3E/components/render/StaticMeshComponent.h"
#include "D3E/engine/ConsoleManager.h"
#include "DisplayWin32.h"
#include "PerObjectConstBuffer.h"
#include "ShaderFactory.h"
#include "Vertex.h"
#include "assetmng/MeshFactory.h"
#include "assetmng/TextureFactory.h"
#include "render/GeometryGenerator.h"
#include "render/systems/StaticMeshInitSystem.h"
#include "render/systems/StaticMeshRenderSystem.h"
#include "assetmng/DefaultAssetLoader.h"
#include <nvrhi/utils.h> // for ClearColorAttachment

void D3E::GameRender::Init(eastl::vector<GameSystem*>& systems)
{
	Debug::LogMessage("[GameRender] Init started");
	if (!device_.Get())
	{
		Debug::LogError("[GameRender] GAPI not initialized");
		Debug::Assert(false, "GAPI not initialized");
	}

	commandList_ = device_->createCommandList();

	auto depthDesc = nvrhi::TextureDesc()
	                     .setDimension(nvrhi::TextureDimension::Texture2D)
	                     .setWidth(display_->ClientWidth)
	                     .setHeight(display_->ClientHeight)
	                     .setFormat(nvrhi::Format::D24S8)
	                     .setInitialState(nvrhi::ResourceStates::DepthWrite)
	                     .setKeepInitialState(true)
	                     .setIsRenderTarget(true)
	                     .setDebugName("Depth Texture");

	nvrhiDepthBuffer = device_->createTexture(depthDesc);

	nvrhi::FramebufferDesc framebufferDesc0 = {};
	framebufferDesc0.addColorAttachment(nvrhiSwapChain[0]);
	framebufferDesc0.setDepthAttachment(nvrhiDepthBuffer);
	nvrhiFramebuffer.push_back(device_->createFramebuffer(framebufferDesc0));

	nvrhi::FramebufferDesc framebufferDesc1 = {};
	framebufferDesc1.addColorAttachment(nvrhiSwapChain[1]);
	framebufferDesc1.setDepthAttachment(nvrhiDepthBuffer);
	nvrhiFramebuffer.push_back(device_->createFramebuffer(framebufferDesc1));

	gbuffer_.Initialize(device_, commandList_, display_.get());
	TextureFactory::RegisterGBuffer(&gbuffer_);

	nvrhi::FramebufferDesc frameGBufferDesc = {};
	frameGBufferDesc.addColorAttachment(gbuffer_.albedoBuffer);
	frameGBufferDesc.addColorAttachment(gbuffer_.positionBuffer);
	frameGBufferDesc.addColorAttachment(gbuffer_.normalBuffer);
	frameGBufferDesc.addColorAttachment(gbuffer_.metalRoughnessBuffer);
	frameGBufferDesc.setDepthAttachment(nvrhiDepthBuffer);
	frameGBuffer = device_->createFramebuffer(frameGBufferDesc);

#ifdef USE_IMGUI
	editor_ = D3E::Editor::Init(device_, display_);
#endif

	ShaderFactory::Initialize(dynamic_cast<Game*>(parentApp));
	MeshFactory::Initialize(dynamic_cast<Game*>(parentApp));

	DefaultAssetLoader::LoadPrimitiveMeshes();
	DefaultAssetLoader::FillPrimitiveMeshBuffers(device_, commandList_);
	DefaultAssetLoader::LoadDefaultPSOs(nvrhiFramebuffer[0], frameGBuffer);
	DefaultAssetLoader::LoadDefaultSamplers(device_);

	for (auto& sys : systems)
	{
		sys->InitRender(commandList_, device_);
	}

	ConsoleManager::getInstance()->registerConsoleVariable("renderingMode", 0);

	/*

	{
		// Unfiltered environment cube map (temporary).
		TextureFactory::GetTextureHandle("environment");


		// Load & convert equirectangular environment map to a cubemap texture.
		{
			ComputeProgram equirectToCubeProgram = createComputeProgram(compileShader("shaders/hlsl/equirect2cube.hlsl", "main", "cs_5_0"));
			Texture envTextureEquirect = createTexture(Image::fromFile("environment.hdr"), DXGI_FORMAT_R32G32B32A32_FLOAT, 1);

			m_context->CSSetShaderResources(0, 1, envTextureEquirect.srv.GetAddressOf());
			m_context->CSSetUnorderedAccessViews(0, 1, envTextureUnfiltered.uav.GetAddressOf(), nullptr);
			m_context->CSSetSamplers(0, 1, m_computeSampler.GetAddressOf());
			m_context->CSSetShader(equirectToCubeProgram.computeShader.Get(), nullptr, 0);
			m_context->Dispatch(envTextureUnfiltered.width/32, envTextureUnfiltered.height/32, 6);
			m_context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
		}

		m_context->GenerateMips(envTextureUnfiltered.srv.Get());

		// Compute pre-filtered specular environment map.
		{
			struct SpecularMapFilterSettingsCB
			{
				float roughness;
				float padding[3];
			};
			ComputeProgram spmapProgram = createComputeProgram(compileShader("shaders/hlsl/spmap.hlsl", "main", "cs_5_0"));
			ComPtr<ID3D11Buffer> spmapCB = createConstantBuffer<SpecularMapFilterSettingsCB>();

			m_envTexture = createTextureCube(1024, 1024, DXGI_FORMAT_R16G16B16A16_FLOAT);

			// Copy 0th mipmap level into destination environment map.
			for(int arraySlice=0; arraySlice<6; ++arraySlice) {
				const UINT subresourceIndex = D3D11CalcSubresource(0, arraySlice, m_envTexture.levels);
				m_context->CopySubresourceRegion(m_envTexture.texture.Get(), subresourceIndex, 0, 0, 0, envTextureUnfiltered.texture.Get(), subresourceIndex, nullptr);
			}

			m_context->CSSetShaderResources(0, 1, envTextureUnfiltered.srv.GetAddressOf());
			m_context->CSSetSamplers(0, 1, m_computeSampler.GetAddressOf());
			m_context->CSSetShader(spmapProgram.computeShader.Get(), nullptr, 0);

			// Pre-filter rest of the mip chain.
			const float deltaRoughness = 1.0f / glm::max(float(m_envTexture.levels-1), 1.0f);
			for(UINT level=1, size=512; level<m_envTexture.levels; ++level, size/=2) {
				const UINT numGroups = glm::max<UINT>(1, size/32);
				createTextureUAV(m_envTexture, level);

				const SpecularMapFilterSettingsCB spmapConstants = { level * deltaRoughness };
				m_context->UpdateSubresource(spmapCB.Get(), 0, nullptr, &spmapConstants, 0, 0);

				m_context->CSSetConstantBuffers(0, 1, spmapCB.GetAddressOf());
				m_context->CSSetUnorderedAccessViews(0, 1, m_envTexture.uav.GetAddressOf(), nullptr);
				m_context->Dispatch(numGroups, numGroups, 6);
			}
			m_context->CSSetConstantBuffers(0, 1, nullBuffer);
			m_context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
		}
	}

	// Compute diffuse irradiance cubemap.
	{
		ComputeProgram irmapProgram = createComputeProgram(compileShader("shaders/hlsl/irmap.hlsl", "main", "cs_5_0"));

		m_irmapTexture = createTextureCube(32, 32, DXGI_FORMAT_R16G16B16A16_FLOAT, 1);
		createTextureUAV(m_irmapTexture, 0);

		m_context->CSSetShaderResources(0, 1, m_envTexture.srv.GetAddressOf());
		m_context->CSSetSamplers(0, 1, m_computeSampler.GetAddressOf());
		m_context->CSSetUnorderedAccessViews(0, 1, m_irmapTexture.uav.GetAddressOf(), nullptr);
		m_context->CSSetShader(irmapProgram.computeShader.Get(), nullptr, 0);
		m_context->Dispatch(m_irmapTexture.width/32, m_irmapTexture.height/32, 6);
		m_context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
	}

	// Compute Cook-Torrance BRDF 2D LUT for split-sum approximation.
	{
		ComputeProgram spBRDFProgram = createComputeProgram(compileShader("shaders/hlsl/spbrdf.hlsl", "main", "cs_5_0"));

		m_spBRDF_LUT = createTexture(256, 256, DXGI_FORMAT_R16G16_FLOAT, 1);
		m_spBRDF_Sampler = createSamplerState(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
		createTextureUAV(m_spBRDF_LUT, 0);

		m_context->CSSetUnorderedAccessViews(0, 1, m_spBRDF_LUT.uav.GetAddressOf(), nullptr);
		m_context->CSSetShader(spBRDFProgram.computeShader.Get(), nullptr, 0);
		m_context->Dispatch(m_spBRDF_LUT.width/32, m_spBRDF_LUT.height/32, 1);
		m_context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
	}*/

	Debug::LogMessage("[GameRender] Init finished");
}

void D3E::GameRender::DestroyResources()
{
//	editor_->Release();
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
	assert(parentApp != nullptr);
	assert(hInstance != nullptr);
	display_ = eastl::make_shared<DisplayWin32>(reinterpret_cast<LPCWSTR>(parentApp->GetName().c_str()), hInstance, 1280, 720, parent);
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

void D3E::GameRender::Draw(entt::registry& registry, eastl::vector<GameSystem*>& systems, eastl::vector<GameSystem*>& renderPPSystems)
{
	// Obtain the current framebuffer from the graphics API
	nvrhi::IFramebuffer* currentFramebuffer = nvrhiFramebuffer[GetCurrentFrameBuffer()];

	commandList_->open();

	// Clear the primary render target
	nvrhi::utils::ClearColorAttachment(commandList_, currentFramebuffer, 0, nvrhi::Color(0.2f));
	//nvrhi::utils::ClearColorAttachment(commandList_, frameGBuffer, 0, nvrhi::Color(0.0f));
	commandList_->clearTextureFloat(gbuffer_.albedoBuffer, nvrhi::AllSubresources, nvrhi::Color(0.0f));
	commandList_->clearTextureFloat(gbuffer_.positionBuffer, nvrhi::AllSubresources, nvrhi::Color(0.0f));
	commandList_->clearTextureFloat(gbuffer_.normalBuffer, nvrhi::AllSubresources, nvrhi::Color(0.0f));
	commandList_->clearTextureFloat(gbuffer_.metalRoughnessBuffer, nvrhi::AllSubresources, nvrhi::Color(0.0f));
	commandList_->clearDepthStencilTexture(nvrhiDepthBuffer, nvrhi::AllSubresources, true, 1.0f, true, 0U);

	for (auto& sys : systems)
	{
		sys->Draw(registry, frameGBuffer, commandList_, device_);
	}

	for (auto& sys : renderPPSystems)
	{
		sys->Draw(registry, currentFramebuffer, commandList_, device_);
	}

	// Close and execute the command list
	commandList_->close();
	device_->executeCommandList(commandList_);
}

void D3E::GameRender::PrepareDraw(entt::registry& registry, eastl::vector<GameSystem*>& systems, eastl::vector<GameSystem*>& renderPPSystems)
{
	for (auto& sys : systems)
	{
		sys->PreDraw(registry, commandList_, device_);
	}

	for (auto& sys : renderPPSystems)
	{
		sys->PreDraw(registry, commandList_, device_);
	}
}

void D3E::GameRender::EndDraw(entt::registry& registry, eastl::vector<GameSystem*>& systems, eastl::vector<GameSystem*>& renderPPSystems)
{
	for (auto& sys : systems)
	{
		sys->PostDraw(registry, commandList_, device_);
	}

	for (auto& sys : renderPPSystems)
	{
		sys->PostDraw(registry, commandList_, device_);
	}
#ifdef USE_IMGUI
	editor_->EndDraw(nvrhiFramebuffer[GetCurrentFrameBuffer()]);
#endif // USE_IMGUI
}

//void D3E::GameRender::LoadTexture(const String& name,
//                                  const String& fileName)
//{
//	TextureFactory::LoadTexture(name, fileName, device_, commandList_);
//}

void D3E::GameRender::UpdateAnimations(float dT)
{
#ifdef USE_IMGUI
	editor_->BeginDraw(dT);
#endif // USE_IMGUI
}
