
/* Adapted from NVIDIA donut framework */

/*
* Copyright (c) 2014-2021, NVIDIA CORPORATION. All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
 */

/*
License for Dear ImGui

Copyright (c) 2014-2019 Omar Cornut

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <cstddef>

#include <imgui.h>

#include "nvrhi/nvrhi.h"

#include "imgui_nvrhi.h"

#include "render/ShaderFactory.h"

struct VERTEX_CONSTANT_BUFFER
{
    float        mvp[4][4];
};

bool D3E::ImGui_NVRHI::createFontTexture(nvrhi::ICommandList* commandList)
{
    ImGuiIO& io = ImGui::GetIO();
    unsigned char *pixels;
    int width, height;

    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    {
        nvrhi::TextureDesc desc;
        desc.width = width;
        desc.height = height;
        desc.format = nvrhi::Format::RGBA8_UNORM;
        desc.debugName = "ImGui font texture";

        fontTexture = renderer->createTexture(desc);

        commandList->beginTrackingTextureState(fontTexture, nvrhi::AllSubresources, nvrhi::ResourceStates::Common);

        if (fontTexture == nullptr)
            return false;

        commandList->writeTexture(fontTexture, 0, 0, pixels, width * 4);

        commandList->setPermanentTextureState(fontTexture, nvrhi::ResourceStates::ShaderResource);
        commandList->commitBarriers();

        io.Fonts->TexID = fontTexture;
    }

    {
        const auto desc = nvrhi::SamplerDesc()
                              .setAllAddressModes(nvrhi::SamplerAddressMode::Wrap)
                              .setAllFilters(true);

        fontSampler = renderer->createSampler(desc);

        if (fontSampler == nullptr)
            return false;
    }

    return true;
}

bool D3E::ImGui_NVRHI::init(const nvrhi::DeviceHandle& renderer)
{
    this->renderer = renderer;

    m_commandList = renderer->createCommandList();

    m_commandList->open();

    vertexShader = ShaderFactory::AddVertexShader("imgui_vertex", "imgui_vertex.hlsl", "main");

    if (vertexShader == nullptr)
    {
        printf("error creating NVRHI vertex shader object\n");
        assert(0);
        return false;
    }

    pixelShader = ShaderFactory::AddPixelShader("imgui_pixel", "imgui_pixel.hlsl", "main");

    if (pixelShader == nullptr)
    {
        printf("error creating NVRHI pixel shader object\n");
        assert(0);
        return false;
    }

    // create attribute layout object
    nvrhi::VertexAttributeDesc vertexAttribLayout[] = {
        { "POSITION", nvrhi::Format::RG32_FLOAT,  1, 0, static_cast<uint32_t>(offsetof(ImDrawVert, pos)), sizeof(ImDrawVert), false },
        { "TEXCOORD", nvrhi::Format::RG32_FLOAT,  1, 0, static_cast<uint32_t>(offsetof(ImDrawVert, uv)),  sizeof(ImDrawVert), false },
        { "COLOR",    nvrhi::Format::RGBA8_UNORM, 1, 0, static_cast<uint32_t>(offsetof(ImDrawVert, col)), sizeof(ImDrawVert), false },
    };

    shaderAttribLayout = renderer->createInputLayout(vertexAttribLayout, sizeof(vertexAttribLayout) / sizeof(vertexAttribLayout[0]), vertexShader);

    // add the default font - before creating the font texture
    auto& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();

    // create font texture
    if (!createFontTexture(m_commandList))
    {
        return false;
    }

    // create PSO
    {
        nvrhi::BlendState blendState;
        blendState.targets[0].setBlendEnable(true)
            .setSrcBlend(nvrhi::BlendFactor::SrcAlpha)
            .setDestBlend(nvrhi::BlendFactor::InvSrcAlpha)
            .setSrcBlendAlpha(nvrhi::BlendFactor::InvSrcAlpha)
            .setDestBlendAlpha(nvrhi::BlendFactor::Zero);

        auto rasterState = nvrhi::RasterState()
                               .setFillSolid()
                               .setCullNone()
                               .setScissorEnable(true)
                               .setDepthClipEnable(true);

        auto depthStencilState = nvrhi::DepthStencilState()
                                     .disableDepthTest()
                                     .enableDepthWrite()
                                     .disableStencil()
                                     .setDepthFunc(nvrhi::ComparisonFunc::Always);

        nvrhi::RenderState renderState;
        renderState.blendState = blendState;
        renderState.depthStencilState = depthStencilState;
        renderState.rasterState = rasterState;

        nvrhi::BindingLayoutDesc layoutDesc;
        layoutDesc.visibility = nvrhi::ShaderType::All;
        layoutDesc.bindings = {
            nvrhi::BindingLayoutItem::PushConstants(0, sizeof(float) * 2),
            nvrhi::BindingLayoutItem::Texture_SRV(0),
            nvrhi::BindingLayoutItem::Sampler(0)
        };
        bindingLayout = renderer->createBindingLayout(layoutDesc);

        basePSODesc.primType = nvrhi::PrimitiveType::TriangleList;
        basePSODesc.inputLayout = shaderAttribLayout;
        basePSODesc.VS = vertexShader;
        basePSODesc.PS = pixelShader;
        basePSODesc.renderState = renderState;
        basePSODesc.bindingLayouts = { bindingLayout };
    }

    m_commandList->close();
    renderer->executeCommandList(m_commandList);

    return true;
}

bool D3E::ImGui_NVRHI::reallocateBuffer(nvrhi::BufferHandle& buffer, size_t requiredSize, size_t reallocateSize, const bool indexBuffer)
{
    if (buffer == nullptr || size_t(buffer->getDesc().byteSize) < requiredSize)
    {
        nvrhi::BufferDesc desc;
        desc.byteSize = uint32_t(reallocateSize);
        desc.structStride = 0;
        desc.debugName = indexBuffer ? "ImGui index buffer" : "ImGui vertex buffer";
        desc.canHaveUAVs = false;
        desc.isVertexBuffer = !indexBuffer;
        desc.isIndexBuffer = indexBuffer;
        desc.isDrawIndirectArgs = false;
        desc.isVolatile = false;
        desc.initialState = indexBuffer ? nvrhi::ResourceStates::IndexBuffer : nvrhi::ResourceStates::VertexBuffer;
        desc.keepInitialState = true;

        buffer = renderer->createBuffer(desc);

        if (!buffer)
        {
            return false;
        }
    }

    return true;
}

bool D3E::ImGui_NVRHI::beginFrame(float elapsedTimeSeconds)
{
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = elapsedTimeSeconds;
    io.MouseDrawCursor = false;

    ImGui::NewFrame();

    return true;
}

nvrhi::IGraphicsPipeline* D3E::ImGui_NVRHI::getPSO(nvrhi::IFramebuffer* fb)
{
    if (pso)
        return pso;

    pso = renderer->createGraphicsPipeline(basePSODesc, fb);
    assert(pso);

    return pso;
}

nvrhi::IBindingSet* D3E::ImGui_NVRHI::getBindingSet(nvrhi::ITexture* texture)
{
    auto iter = bindingsCache.find(texture);
    if (iter != bindingsCache.end())
    {
        return iter->second;
    }

    nvrhi::BindingSetDesc desc;

    desc.bindings = {
        nvrhi::BindingSetItem::PushConstants(0, sizeof(float) * 2),
        nvrhi::BindingSetItem::Texture_SRV(0, texture),
        nvrhi::BindingSetItem::Sampler(0, fontSampler)
    };

    nvrhi::BindingSetHandle binding;
    binding = renderer->createBindingSet(desc, bindingLayout);
    assert(binding);

    bindingsCache[texture] = binding;
    return binding;
}

bool D3E::ImGui_NVRHI::updateGeometry(nvrhi::ICommandList* commandList)
{
    ImDrawData *drawData = ImGui::GetDrawData();

    // create/resize vertex and index buffers if needed
    if (!reallocateBuffer(vertexBuffer,
                          drawData->TotalVtxCount * sizeof(ImDrawVert),
                          (drawData->TotalVtxCount + 5000) * sizeof(ImDrawVert),
                          false))
    {
        return false;
    }

    if (!reallocateBuffer(indexBuffer,
                          drawData->TotalIdxCount * sizeof(ImDrawIdx),
                          (drawData->TotalIdxCount + 5000) * sizeof(ImDrawIdx),
                          true))
    {
        return false;
    }

    vtxBuffer.resize(vertexBuffer->getDesc().byteSize / sizeof(ImDrawVert));
    idxBuffer.resize(indexBuffer->getDesc().byteSize / sizeof(ImDrawIdx));

    // copy and convert all vertices into a single contiguous buffer
    ImDrawVert *vtxDst = &vtxBuffer[0];
    ImDrawIdx *idxDst = &idxBuffer[0];

    for(int n = 0; n < drawData->CmdListsCount; n++)
    {
        const ImDrawList *cmdList = drawData->CmdLists[n];

        memcpy(vtxDst, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(idxDst, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));

        vtxDst += cmdList->VtxBuffer.Size;
        idxDst += cmdList->IdxBuffer.Size;
    }

    commandList->writeBuffer(vertexBuffer, &vtxBuffer[0], vertexBuffer->getDesc().byteSize);
    commandList->writeBuffer(indexBuffer, &idxBuffer[0], indexBuffer->getDesc().byteSize);

    return true;
}

bool D3E::ImGui_NVRHI::render(nvrhi::IFramebuffer* framebuffer)
{
    ImDrawData *drawData = ImGui::GetDrawData();
    const auto& io = ImGui::GetIO();

    m_commandList->open();
    m_commandList->beginMarker("ImGUI");

    if (!updateGeometry(m_commandList))
    {
        return false;
    }

    // handle DPI scaling
    drawData->ScaleClipRects(io.DisplayFramebufferScale);

    float invDisplaySize[2] = { 1.f / io.DisplaySize.x, 1.f / io.DisplaySize.y };

    // set up graphics state
    nvrhi::GraphicsState drawState;

    drawState.framebuffer = framebuffer;
    assert(drawState.framebuffer);

    drawState.pipeline = getPSO(drawState.framebuffer);

    drawState.viewport.viewports.push_back(nvrhi::Viewport(io.DisplaySize.x * io.DisplayFramebufferScale.x,
                                                           io.DisplaySize.y * io.DisplayFramebufferScale.y));
    drawState.viewport.scissorRects.resize(1);  // updated below

    nvrhi::VertexBufferBinding vbufBinding;
    vbufBinding.buffer = vertexBuffer;
    vbufBinding.slot = 0;
    vbufBinding.offset = 0;
    drawState.vertexBuffers.push_back(vbufBinding);

    drawState.indexBuffer.buffer = indexBuffer;
    drawState.indexBuffer.format = (sizeof(ImDrawIdx) == 2 ? nvrhi::Format::R16_UINT : nvrhi::Format::R32_UINT);
    drawState.indexBuffer.offset = 0;

    // render command lists
    int vtxOffset = 0;
    int idxOffset = 0;
    for(int n = 0; n < drawData->CmdListsCount; n++)
    {
        const ImDrawList *cmdList = drawData->CmdLists[n];
        for(int i = 0; i < cmdList->CmdBuffer.Size; i++)
        {
            const ImDrawCmd *pCmd = &cmdList->CmdBuffer[i];

            if (pCmd->UserCallback)
            {
                pCmd->UserCallback(cmdList, pCmd);
            } else {
                drawState.bindings = { getBindingSet((nvrhi::ITexture*)pCmd->TextureId) };
                assert(drawState.bindings[0]);

                drawState.viewport.scissorRects[0] = nvrhi::Rect(int(pCmd->ClipRect.x),
                                                                 int(pCmd->ClipRect.z),
                                                                 int(pCmd->ClipRect.y),
                                                                 int(pCmd->ClipRect.w));

                nvrhi::DrawArguments drawArguments;
                drawArguments.vertexCount = pCmd->ElemCount;
                drawArguments.startIndexLocation = idxOffset;
                drawArguments.startVertexLocation = vtxOffset;

                m_commandList->setGraphicsState(drawState);
                m_commandList->setPushConstants(invDisplaySize, sizeof(invDisplaySize));
                m_commandList->drawIndexed(drawArguments);
            }

            idxOffset += pCmd->ElemCount;
        }

        vtxOffset += cmdList->VtxBuffer.Size;
    }

    m_commandList->endMarker();
    m_commandList->close();
    renderer->executeCommandList(m_commandList);

    return true;
}

void D3E::ImGui_NVRHI::backbufferResizing()
{
    pso = nullptr;
}
