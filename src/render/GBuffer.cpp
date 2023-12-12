#include "GBuffer.h"

#include "Display.h"

void D3E::GBuffer::Initialize(nvrhi::IDevice* device,
                         nvrhi::ICommandList* commandList,
                         Display* display)
{
	nvrhi::TextureDesc albedoDesc = {};
	albedoDesc.format = nvrhi::Format::RGBA8_UNORM;
	albedoDesc.setWidth(display->ClientWidth);
	albedoDesc.setHeight(display->ClientHeight);
	albedoDesc.isRenderTarget = true;
	albedoDesc.sampleCount = 1;
	albedoDesc.sampleQuality = 0;
	albedoDesc.dimension = nvrhi::TextureDimension::Texture2D;
	albedoDesc.initialState = nvrhi::ResourceStates::RenderTarget;
	albedoDesc.setKeepInitialState(true);
	albedoDesc.setDebugName("Albedo Texture");
	albedoBuffer = device->createTexture(albedoDesc);

	nvrhi::TextureDesc positionDesc = {};
	positionDesc.format = nvrhi::Format::RGBA32_FLOAT;
	positionDesc.setWidth(display->ClientWidth);
	positionDesc.setHeight(display->ClientHeight);
	positionDesc.isRenderTarget = true;
	positionDesc.sampleCount = 1;
	positionDesc.sampleQuality = 0;
	positionDesc.dimension = nvrhi::TextureDimension::Texture2D;
	positionDesc.initialState = nvrhi::ResourceStates::RenderTarget;
	positionDesc.setKeepInitialState(true);
	positionDesc.setDebugName("WorldPos Texture");
	positionBuffer = device->createTexture(positionDesc);

	nvrhi::TextureDesc normalDesc = {};
	normalDesc.format = nvrhi::Format::RGBA32_FLOAT;
	normalDesc.setWidth(display->ClientWidth);
	normalDesc.setHeight(display->ClientHeight);
	normalDesc.isRenderTarget = true;
	normalDesc.sampleCount = 1;
	normalDesc.sampleQuality = 0;
	normalDesc.dimension = nvrhi::TextureDimension::Texture2D;
	normalDesc.initialState = nvrhi::ResourceStates::RenderTarget;
	normalDesc.setKeepInitialState(true);
	normalDesc.setDebugName("Normal Texture");
	normalBuffer = device->createTexture(normalDesc);

	nvrhi::TextureDesc metalRoughnessDesc = {};
	metalRoughnessDesc.format = nvrhi::Format::RGBA32_FLOAT;
	metalRoughnessDesc.setWidth(display->ClientWidth);
	metalRoughnessDesc.setHeight(display->ClientHeight);
	metalRoughnessDesc.isRenderTarget = true;
	metalRoughnessDesc.sampleCount = 1;
	metalRoughnessDesc.sampleQuality = 0;
	metalRoughnessDesc.dimension = nvrhi::TextureDimension::Texture2D;
	metalRoughnessDesc.initialState = nvrhi::ResourceStates::RenderTarget;
	metalRoughnessDesc.setKeepInitialState(true);
	metalRoughnessDesc.setDebugName("MetalRoughness Texture");
	metalRoughnessBuffer = device->createTexture(metalRoughnessDesc);

	nvrhi::TextureDesc editorIdsDesc = {};
	editorIdsDesc.format = nvrhi::Format::R32_UINT;
	editorIdsDesc.setWidth(display->ClientWidth);
	editorIdsDesc.setHeight(display->ClientHeight);
	editorIdsDesc.isRenderTarget = true;
	editorIdsDesc.sampleCount = 1;
	editorIdsDesc.sampleQuality = 0;
	editorIdsDesc.dimension = nvrhi::TextureDimension::Texture2D;
	editorIdsDesc.initialState = nvrhi::ResourceStates::RenderTarget;
	editorIdsDesc.setKeepInitialState(true);
	editorIdsDesc.setDebugName("EditorIds Texture");
	editorIdsBuffer = device->createTexture(editorIdsDesc);
}
