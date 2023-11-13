// dear imgui: Renderer Backend for nvrhi
// This needs to be used along with a Platform Backend (e.g. Win32)

#include "imgui.h"
#ifndef IMGUI_DISABLE
#include "imgui_impl_nvrhi.h"

#include "nvrhi/nvrhi.h"

//#include <d3d12.h>
//#include <dxgi1_4.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler")

struct ImGui_ImplNVRHI_RenderBuffers;
struct ImGui_ImplNVRHI_Data
{
	nvrhi::IDevice*                pnvrhiDevice;
	nvrhi::GraphicsState           pPipelineState;
	nvrhi::Format                  RTVFormat;
	nvrhi::ITexture*               pFontTextureResource;
	UINT                           numFramesInFlight;
	ImGui_ImplNVRHI_RenderBuffers* pFrameResources;
	UINT                           frameIndex;

	ImGui_ImplNVRHI_Data()       { memset((void*)this, 0, sizeof(*this)); frameIndex = UINT_MAX; }
};

// Backend data stored in io.BackendRendererUserData to allow support for multiple Dear ImGui contexts
// It is STRONGLY preferred that you use docking branch with multi-viewports (== single Dear ImGui context + multiple windows) instead of multiple Dear ImGui contexts.
static ImGui_ImplNVRHI_Data* ImGui_ImplNVRHI_GetBackendData()
{
	return ImGui::GetCurrentContext() ? (ImGui_ImplNVRHI_Data*)ImGui::GetIO().BackendRendererUserData : nullptr;
}

// Buffers used during the rendering of a frame
struct ImGui_ImplNVRHI_RenderBuffers
{
	nvrhi::IBuffer*     IndexBuffer;
	nvrhi::IBuffer*     VertexBuffer;
	int                 IndexBufferSize;
	int                 VertexBufferSize;
};

struct VERTEX_CONSTANT_BUFFER_NVRHI
{
	float mvp[4][4];
};

// Functions
static void ImGui_ImplNVRHI_SetupRenderState(ImDrawData* draw_data, nvrhi::ICommandList* ctx, ImGui_ImplNVRHI_RenderBuffers* fr)
{
	ImGui_ImplNVRHI_Data* bd = ImGui_ImplNVRHI_GetBackendData();

	// Setup orthographic projection matrix into our constant buffer
	// Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right).
	VERTEX_CONSTANT_BUFFER_NVRHI vertex_constant_buffer;
	{
		float L = draw_data->DisplayPos.x;
		float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
		float T = draw_data->DisplayPos.y;
		float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
		float mvp[4][4] =
			{
				{ 2.0f/(R-L),   0.0f,           0.0f,       0.0f },
				{ 0.0f,         2.0f/(T-B),     0.0f,       0.0f },
				{ 0.0f,         0.0f,           0.5f,       0.0f },
				{ (R+L)/(L-R),  (T+B)/(B-T),    0.5f,       1.0f },
			};
		memcpy(&vertex_constant_buffer.mvp, mvp, sizeof(mvp));
	}

	// Setup viewport
	nvrhi::Viewport vp(draw_data->DisplaySize.x, draw_data->DisplaySize.y);
	auto vps = nvrhi::ViewportState().addViewportAndScissorRect(vp);

	ctx->setGraphicsState(bd->pPipelineState);

	auto graphicsState = nvrhi::GraphicsState()
	                         .setPipeline()
	                         .setFramebuffer()
	                         .setViewport(vps)
	                         .addBindingSet()
	                         .addBindingSet()
	                         .addVertexBuffer()
	                         .setIndexBuffer();

	ctx->setGraphicsState(graphicsState);
}

// Render function
void ImGui_ImplNVRHI_RenderDrawData(ImDrawData* draw_data, nvrhi::ICommandList* ctx)
{
	// Avoid rendering when minimized
	if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
		return;

	// FIXME: I'm assuming that this only gets called once per frame!
	// If not, we can't just re-allocate the IB or VB, we'll have to do a proper allocator.
	ImGui_ImplNVRHI_Data* bd = ImGui_ImplNVRHI_GetBackendData();
	bd->frameIndex = bd->frameIndex + 1;
	ImGui_ImplNVRHI_RenderBuffers* fr = &bd->pFrameResources[bd->frameIndex % bd->numFramesInFlight];

	/*
	// Create and grow vertex/index buffers if needed
	if (fr->VertexBuffer == nullptr || fr->VertexBufferSize < draw_data->TotalVtxCount)
	{
		SafeRelease(fr->VertexBuffer);
		fr->VertexBufferSize = draw_data->TotalVtxCount + 5000;
		D3D12_HEAP_PROPERTIES props;
		memset(&props, 0, sizeof(D3D12_HEAP_PROPERTIES));
		props.Type = D3D12_HEAP_TYPE_UPLOAD;
		props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		D3D12_RESOURCE_DESC desc;
		memset(&desc, 0, sizeof(D3D12_RESOURCE_DESC));
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Width = fr->VertexBufferSize * sizeof(ImDrawVert);
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		if (bd->pd3dDevice->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&fr->VertexBuffer)) < 0)
			return;
	}
	if (fr->IndexBuffer == nullptr || fr->IndexBufferSize < draw_data->TotalIdxCount)
	{
		SafeRelease(fr->IndexBuffer);
		fr->IndexBufferSize = draw_data->TotalIdxCount + 10000;
		D3D12_HEAP_PROPERTIES props;
		memset(&props, 0, sizeof(D3D12_HEAP_PROPERTIES));
		props.Type = D3D12_HEAP_TYPE_UPLOAD;
		props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		D3D12_RESOURCE_DESC desc;
		memset(&desc, 0, sizeof(D3D12_RESOURCE_DESC));
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Width = fr->IndexBufferSize * sizeof(ImDrawIdx);
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		if (bd->pd3dDevice->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&fr->IndexBuffer)) < 0)
			return;
	}

	// Upload vertex/index data into a single contiguous GPU buffer
	void* vtx_resource, *idx_resource;
	D3D12_RANGE range;
	memset(&range, 0, sizeof(D3D12_RANGE));
	if (fr->VertexBuffer->Map(0, &range, &vtx_resource) != S_OK)
		return;
	if (fr->IndexBuffer->Map(0, &range, &idx_resource) != S_OK)
		return;
	ImDrawVert* vtx_dst = (ImDrawVert*)vtx_resource;
	ImDrawIdx* idx_dst = (ImDrawIdx*)idx_resource;
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		vtx_dst += cmd_list->VtxBuffer.Size;
		idx_dst += cmd_list->IdxBuffer.Size;
	}
	fr->VertexBuffer->Unmap(0, &range);
	fr->IndexBuffer->Unmap(0, &range);
	 */

	// Setup desired DX state
	ImGui_ImplNVRHI_SetupRenderState(draw_data, ctx, fr);

	// Render command lists
	// (Because we merged all buffers into a single one, we maintain our own offset into them)
	int global_vtx_offset = 0;
	int global_idx_offset = 0;
	ImVec2 clip_off = draw_data->DisplayPos;
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback != nullptr)
			{
				// User callback, registered via ImDrawList::AddCallback()
				// (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
				if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
					ImGui_ImplNVRHI_SetupRenderState(draw_data, ctx, fr);
				else
					pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				// Project scissor/clipping rectangles into framebuffer space
				ImVec2 clip_min(pcmd->ClipRect.x - clip_off.x, pcmd->ClipRect.y - clip_off.y);
				ImVec2 clip_max(pcmd->ClipRect.z - clip_off.x, pcmd->ClipRect.w - clip_off.y);
				if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
					continue;

//				// Apply Scissor/clipping rectangle, Bind texture, Draw
//				const D3D12_RECT r = { (LONG)clip_min.x, (LONG)clip_min.y, (LONG)clip_max.x, (LONG)clip_max.y };
//				D3D12_GPU_DESCRIPTOR_HANDLE texture_handle = {};
//				texture_handle.ptr = (UINT64)pcmd->GetTexID();
//				ctx->SetGraphicsRootDescriptorTable(1, texture_handle);

				auto drawArguments = nvrhi::DrawArguments()
				                        .setVertexCount(pcmd->ElemCount)
										.setInstanceCount(1)
										.setStartIndexLocation(pcmd->IdxOffset + global_idx_offset)
										.setStartVertexLocation(pcmd->VtxOffset + global_vtx_offset)
										.setStartInstanceLocation(0);
				ctx->drawIndexed(drawArguments);
			}
		}
		global_idx_offset += cmd_list->IdxBuffer.Size;
		global_vtx_offset += cmd_list->VtxBuffer.Size;
	}
}

static void ImGui_ImplNVRHI_CreateFontsTexture()
{
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplNVRHI_Data* bd = ImGui_ImplNVRHI_GetBackendData();
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	// Upload texture to graphics system
	{
		nvrhi::TextureDesc desc = {};
		desc.dimension = nvrhi::TextureDimension::Texture2D;
		desc.width = width;
		desc.height = height;
		desc.arraySize = 1;
		desc.mipLevels = 1;
		desc.format = nvrhi::Format::RGBA8_UNORM;
		desc.sampleCount = 1;
		desc.sampleQuality = 0;

		nvrhi::TextureHandle pTexture = bd->pnvrhiDevice->createTexture(desc);

		bd->pFontTextureResource = nullptr;
		bd->pFontTextureResource = pTexture;

		io.Fonts->SetTexID((ImTextureID)bd->pFontTextureResource);
	}
}

bool ImGui_ImplNVRHI_CreateDeviceObjects()
{
	ImGui_ImplNVRHI_Data* bd = ImGui_ImplNVRHI_GetBackendData();
	if (!bd || !bd->pnvrhiDevice)
		return false;

	{
		nvrhi::SamplerDesc staticSampler = {};
		staticSampler.minFilter = true;
		staticSampler.mipFilter = true;
		staticSampler.magFilter = true;
		staticSampler.addressU = nvrhi::SamplerAddressMode::Wrap;
		staticSampler.addressV = nvrhi::SamplerAddressMode::Wrap;
		staticSampler.addressW = nvrhi::SamplerAddressMode::Wrap;
		staticSampler.mipBias = 0.f;
		staticSampler.maxAnisotropy = 0;
		staticSampler.borderColor = nvrhi::Color(0.f, 0.f, 0.f, 0.f);
	}

	// By using D3DCompile() from <d3dcompiler.h> / d3dcompiler.lib, we introduce a dependency to a given version of d3dcompiler_XX.dll (see D3DCOMPILER_DLL_A)
	// If you would like to use this DX12 sample code but remove this dependency you can:
	//  1) compile once, save the compiled shader blobs into a file or source code and assign them to psoDesc.VS/PS [preferred solution]
	//  2) use code to detect any version of the DLL and grab a pointer to D3DCompile from the DLL.
	// See https://github.com/ocornut/imgui/pull/638 for sources and details.

	/*D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	psoDesc.NodeMask = 1;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.pRootSignature = bd->pRootSignature;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = bd->RTVFormat;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;*/

	nvrhi::GraphicsPipelineDesc pipelineDesc = {};
	pipelineDesc.primType = nvrhi::PrimitiveType::TriangleList;

	ID3DBlob* vertexShaderBlob;
	ID3DBlob* pixelShaderBlob;

	// Create the vertex shader
	{
		static const char* vertexShader =
			"cbuffer vertexBuffer : register(b0) \
            {\
              float4x4 ProjectionMatrix; \
            };\
            struct VS_INPUT\
            {\
              float2 pos : POSITION;\
              float4 col : COLOR0;\
              float2 uv  : TEXCOORD0;\
            };\
            \
            struct PS_INPUT\
            {\
              float4 pos : SV_POSITION;\
              float4 col : COLOR0;\
              float2 uv  : TEXCOORD0;\
            };\
            \
            PS_INPUT main(VS_INPUT input)\
            {\
              PS_INPUT output;\
              output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));\
              output.col = input.col;\
              output.uv  = input.uv;\
              return output;\
            }";

		if (FAILED(D3DCompile(vertexShader, strlen(vertexShader), nullptr, nullptr, nullptr, "main", "vs_5_0", 0, 0, &vertexShaderBlob, nullptr)))
			return false; // NB: Pass ID3DBlob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!

		pipelineDesc.setVertexShader(bd->pnvrhiDevice->createShader(nvrhi::ShaderDesc(nvrhi::ShaderType::Vertex), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize()));

		// Create the input layout
		nvrhi::VertexAttributeDesc local_layout[] = {
			nvrhi::VertexAttributeDesc()
				.setName("POSITION")
				.setFormat(nvrhi::Format::RG32_FLOAT)
				.setOffset((UINT)offsetof(ImDrawVert, pos))
				.setElementStride(sizeof(ImDrawVert)),
			nvrhi::VertexAttributeDesc()
				.setName("TEXCOORD")
				.setFormat(nvrhi::Format::RG32_FLOAT)
				.setOffset((UINT)offsetof(ImDrawVert, uv))
				.setElementStride(sizeof(ImDrawVert)),
			nvrhi::VertexAttributeDesc()
				.setName("COLOR")
				.setFormat(nvrhi::Format::RGBA8_UNORM)
				.setOffset((UINT)offsetof(ImDrawVert, col))
				.setElementStride(sizeof(ImDrawVert)),
		};
		pipelineDesc.setInputLayout(bd->pnvrhiDevice->createInputLayout(local_layout, 3, pipelineDesc.VS));

		pipelineDesc.addBindingLayout();
		pipelineDesc.addBindingLayout();
	}

	// Create the pixel shader
	{
		static const char* pixelShader =
			"struct PS_INPUT\
            {\
              float4 pos : SV_POSITION;\
              float4 col : COLOR0;\
              float2 uv  : TEXCOORD0;\
            };\
            SamplerState sampler0 : register(s0);\
            Texture2D texture0 : register(t0);\
            \
            float4 main(PS_INPUT input) : SV_Target\
            {\
              float4 out_col = input.col * texture0.Sample(sampler0, input.uv); \
              return out_col; \
            }";

		if (FAILED(D3DCompile(pixelShader, strlen(pixelShader), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, &pixelShaderBlob, nullptr)))
		{
			vertexShaderBlob->Release();
			return false; // NB: Pass ID3DBlob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
		}
		pipelineDesc.setPixelShader(bd->pnvrhiDevice->createShader(nvrhi::ShaderDesc(nvrhi::ShaderType::Pixel), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize()));
	}

	// Create the blending setup
	{
		pipelineDesc.renderState.blendState = {};
		pipelineDesc.renderState.blendState.alphaToCoverageEnable = false;
		pipelineDesc.renderState.blendState.targets[0].blendEnable = true;
		pipelineDesc.renderState.blendState.targets[0].srcBlend = nvrhi::BlendFactor::SrcAlpha;
		pipelineDesc.renderState.blendState.targets[0].destBlend = nvrhi::BlendFactor::InvSrcAlpha;
		pipelineDesc.renderState.blendState.targets[0].blendOp = nvrhi::BlendOp::Add;
		pipelineDesc.renderState.blendState.targets[0].srcBlendAlpha = nvrhi::BlendFactor::One;
		pipelineDesc.renderState.blendState.targets[0].destBlendAlpha = nvrhi::BlendFactor::InvSrcAlpha;
		pipelineDesc.renderState.blendState.targets[0].blendOpAlpha = nvrhi::BlendOp::Add;
		pipelineDesc.renderState.blendState.targets[0].colorWriteMask = nvrhi::ColorMask::All;
	}

	// Create the rasterizer state
	{
		pipelineDesc.renderState.rasterState = {};
		pipelineDesc.renderState.rasterState.fillMode = nvrhi::RasterFillMode::Solid;
		pipelineDesc.renderState.rasterState.cullMode = nvrhi::RasterCullMode::None;
		pipelineDesc.renderState.rasterState.frontCounterClockwise = false;
		pipelineDesc.renderState.rasterState.depthClipEnable = true;
		pipelineDesc.renderState.rasterState.multisampleEnable = false;
		pipelineDesc.renderState.rasterState.antialiasedLineEnable = false;
		pipelineDesc.renderState.rasterState.forcedSampleCount = 0;
		pipelineDesc.renderState.rasterState.conservativeRasterEnable = false;
	}

	// Create depth-stencil State
	{
		pipelineDesc.renderState.depthStencilState = {};
		pipelineDesc.renderState.depthStencilState.depthTestEnable = false;
		pipelineDesc.renderState.depthStencilState.depthWriteEnable = true;
		pipelineDesc.renderState.depthStencilState.depthFunc = nvrhi::ComparisonFunc::Always;
		pipelineDesc.renderState.depthStencilState.stencilEnable = false;
		//pipelineDesc.renderState.depthStencilState.frontFaceStencil = nvrhi::StencilOp::Keep;
		//pipelineDesc.renderState.depthStencilState.backFaceStencil = nvrhi::StencilOp::Keep;
	}

	bd->pnvrhiDevice->createGraphicsPipeline(pipelineDesc, fb);
	vertexShaderBlob->Release();
	pixelShaderBlob->Release();

	ImGui_ImplNVRHI_CreateFontsTexture();

	return true;
}

void ImGui_ImplNVRHI_InvalidateDeviceObjects()
{
	ImGui_ImplNVRHI_Data* bd = ImGui_ImplNVRHI_GetBackendData();
	if (!bd || !bd->pnvrhiDevice)
		return;

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->SetTexID(0); // We copied bd->pFontTextureView to io.Fonts->TexID so let's clear that as well.

	for (UINT i = 0; i < bd->numFramesInFlight; i++)
	{
		ImGui_ImplNVRHI_RenderBuffers* fr = &bd->pFrameResources[i];
	}
}

bool ImGui_ImplNVRHI_Init(nvrhi::IDevice* device, int num_frames_in_flight, nvrhi::Format rtv_format, /*ID3D12DescriptorHeap* cbv_srv_heap,
                         D3D12_CPU_DESCRIPTOR_HANDLE font_srv_cpu_desc_handle,*/ const nvrhi::TextureDesc& font_srv_gpu_desc_handle)
{
	ImGuiIO& io = ImGui::GetIO();
	IM_ASSERT(io.BackendRendererUserData == nullptr && "Already initialized a renderer backend!");

	// Setup backend capabilities flags
	ImGui_ImplNVRHI_Data* bd = IM_NEW(ImGui_ImplNVRHI_Data)();
	io.BackendRendererUserData = (void*)bd;
	io.BackendRendererName = "imgui_impl_dx12";
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.

	bd->pnvrhiDevice = device;
	bd->RTVFormat = rtv_format;
	//bd->hFontSrvGpuDescHandle = font_srv_gpu_desc_handle;
	bd->pFrameResources = new ImGui_ImplNVRHI_RenderBuffers[num_frames_in_flight];
	bd->numFramesInFlight = num_frames_in_flight;
	bd->frameIndex = UINT_MAX;

	// Create buffers with a default size (they will later be grown as needed)
	for (int i = 0; i < num_frames_in_flight; i++)
	{
		ImGui_ImplNVRHI_RenderBuffers* fr = &bd->pFrameResources[i];
		fr->IndexBuffer = nullptr;
		fr->VertexBuffer = nullptr;
		fr->IndexBufferSize = 10000;
		fr->VertexBufferSize = 5000;
	}

	return true;
}

void ImGui_ImplNVRHI_Shutdown()
{
	ImGui_ImplNVRHI_Data* bd = ImGui_ImplNVRHI_GetBackendData();
	IM_ASSERT(bd != nullptr && "No renderer backend to shutdown, or already shutdown?");
	ImGuiIO& io = ImGui::GetIO();

	// Clean up windows and device objects
	ImGui_ImplNVRHI_InvalidateDeviceObjects();
	delete[] bd->pFrameResources;
	io.BackendRendererName = nullptr;
	io.BackendRendererUserData = nullptr;
	io.BackendFlags &= ~ImGuiBackendFlags_RendererHasVtxOffset;
	IM_DELETE(bd);
}

void ImGui_ImplNVRHI_NewFrame()
{
	ImGui_ImplNVRHI_Data* bd = ImGui_ImplNVRHI_GetBackendData();
	IM_ASSERT(bd != nullptr && "Did you call ImGui_ImplNVRHI_Init()?");

	if (!bd->pPipelineState)
		ImGui_ImplNVRHI_CreateDeviceObjects();
}

//-----------------------------------------------------------------------------

#endif // #ifndef IMGUI_DISABLE
