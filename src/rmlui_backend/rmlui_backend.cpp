#include "rmlui_backend.h"

#include "D3E/Game.h"
#include "assetmng/TextureFactory.h"
#include "core/EngineState.h"
#include "render/GameRender.h"
#include "render/ShaderFactory.h"

void D3E::RmlUi_NVRHI::RenderGeometry(Rml::Vertex* vertices, int num_vertices,
                                      int* indices, int num_indices,
                                      Rml::TextureHandle texture,
                                      const Rml::Vector2f& translation)
{
	Rml::CompiledGeometryHandle handle = CompileGeometry(vertices, num_vertices, indices, num_indices, texture);

	if (handle)
	{
		RenderCompiledGeometry(handle, translation);
		ReleaseCompiledGeometry(handle);
	}
}

Rml::CompiledGeometryHandle D3E::RmlUi_NVRHI::CompileGeometry(Rml::Vertex* vertices, int num_vertices,
                                  int* indices, int num_indices,
                                  Rml::TextureHandle texture)
{
	auto handle = GetNewCompiledGeometryHandle();

	RmlCompiledGeometry compiledGeometry = {};
	compiledGeometry.texture = texture;
	compiledGeometry.numIndices = num_indices;

	eastl::vector<RmlVertex> vData;

	for (int i = 0; i < num_vertices; i++)
	{
		RmlVertex vert = {};
		vert.pos.x = 2.0f * (vertices[i].position.x / static_cast<float>(EngineState::GetGameViewportWidth()) - 0.5f);
		vert.pos.y = 2.0f * (0.5f - vertices[i].position.y / static_cast<float>(EngineState::GetGameViewportHeight()));
		vert.tex.x = vertices[i].tex_coord.x;
		vert.tex.y = vertices[i].tex_coord.y;
		vert.col.x = static_cast<float>(vertices[i].colour.red) / 255.0f;
		vert.col.y = static_cast<float>(vertices[i].colour.green) / 255.0f;
		vert.col.z = static_cast<float>(vertices[i].colour.blue) / 255.0f;
		vert.col.w = static_cast<float>(vertices[i].colour.alpha) / 255.0f;
		vData.push_back(vert);
	}

	auto vertexBufferDesc = nvrhi::BufferDesc()
	                            .setByteSize(num_vertices * sizeof(RmlVertex))
	                            .setIsVertexBuffer(true)
	                            .setInitialState(nvrhi::ResourceStates::VertexBuffer)
	                            .setKeepInitialState(true)
	                            .setDebugName("Vertex Buffer");

	compiledGeometry.vertexBuffer = renderer_->createBuffer(vertexBufferDesc);

	auto indexBufferDesc = nvrhi::BufferDesc()
	                           .setByteSize(num_indices * sizeof(int))
	                           .setIsIndexBuffer(true)
	                           .setInitialState(nvrhi::ResourceStates::IndexBuffer)
	                           .setKeepInitialState(true)
	                           .setDebugName("Index Buffer");

	compiledGeometry.indexBuffer = renderer_->createBuffer(indexBufferDesc);

	commandList_->open();
	commandList_->beginMarker("RmlGui");
	commandList_->writeBuffer(compiledGeometry.vertexBuffer, &(vData[0]), num_vertices * sizeof(RmlVertex));
	commandList_->writeBuffer(compiledGeometry.indexBuffer, indices, num_indices * sizeof(int));
	commandList_->writeBuffer(constantBuffer_, &cbData, sizeof(cbData));
	commandList_->close();
	renderer_->executeCommandList(commandList_);

	compiledGeometry.vertexBufferBinding = {};
	compiledGeometry.vertexBufferBinding.buffer = compiledGeometry.vertexBuffer;
	compiledGeometry.vertexBufferBinding.slot = 0;
	compiledGeometry.vertexBufferBinding.offset = 0;

	compiledGeometry.indexBufferBinding = {};
	compiledGeometry.indexBufferBinding.buffer = compiledGeometry.indexBuffer;
	compiledGeometry.indexBufferBinding.format = nvrhi::Format::R32_UINT;
	compiledGeometry.indexBufferBinding.offset = 0;

	compiledGeometryCache_.insert({handle, compiledGeometry});
	takenCompiledGeometryHandles_.insert(handle);

	return handle;
}

void D3E::RmlUi_NVRHI::RenderCompiledGeometry(
	Rml::CompiledGeometryHandle geometry, const Rml::Vector2f& translation)
{
	assert(compiledGeometryCache_.find(geometry) != compiledGeometryCache_.end());

	cbData.gTranslate.x = 2.0f * translation.x / static_cast<float>(EngineState::GetGameViewportWidth());
	cbData.gTranslate.y = 2.0f * -translation.y / static_cast<float>(EngineState::GetGameViewportHeight());

	commandList_->open();
	commandList_->writeBuffer(constantBuffer_, &cbData, sizeof(cbData));
	commandList_->close();
	renderer_->executeCommandList(commandList_);

	if (compiledGeometryCache_[geometry].texture != 0)
	{
		assert(bindingsCache_.find(compiledGeometryCache_[geometry].texture) != bindingsCache_.end());
		graphicsState_.setPipeline(psoTex_);
		graphicsState_.bindings = {bindingSetV_, bindingsCache_[compiledGeometryCache_[geometry].texture]};
	}
	else
	{
		graphicsState_.setPipeline(psoCol_);
		graphicsState_.bindings = {bindingSetV_, nullBindingSetP_};
	}

	graphicsState_.setFramebuffer(game_->GetRender()->GetGameFramebuffer());
	graphicsState_.vertexBuffers = { compiledGeometryCache_[geometry].vertexBufferBinding };
	graphicsState_.indexBuffer = { compiledGeometryCache_[geometry].indexBufferBinding };

	nvrhi::DrawArguments drawArguments;
	drawArguments.vertexCount = compiledGeometryCache_[geometry].numIndices;
	drawArguments.startIndexLocation = 0;
	drawArguments.startVertexLocation = 0;

	commandList_->open();
	commandList_->setGraphicsState(graphicsState_);
	commandList_->drawIndexed(drawArguments);
	commandList_->endMarker();
	commandList_->close();
	renderer_->executeCommandList(commandList_);
}

void D3E::RmlUi_NVRHI::ReleaseCompiledGeometry(
	Rml::CompiledGeometryHandle geometry)
{
	if (compiledGeometryCache_.find(geometry) != compiledGeometryCache_.end())
	{
		compiledGeometryCache_.erase(geometry);
	}
	if (takenCompiledGeometryHandles_.find(geometry) != takenCompiledGeometryHandles_.end())
	{
		takenCompiledGeometryHandles_.erase(geometry);
	}
}

void D3E::RmlUi_NVRHI::EnableScissorRegion(bool enable)
{
	if (enable)
	{
		viewportState_.scissorRects = { scissorsRect_ };
		return;
	}
	viewportState_.scissorRects = { nvrhi::Rect(0, EngineState::GetGameViewportWidth(), 0, EngineState::GetGameViewportHeight()) };
}

void D3E::RmlUi_NVRHI::SetScissorRegion(int x, int y, int width, int height)
{
	scissorsRect_.minX = x;
	scissorsRect_.minY = y;
	scissorsRect_.maxX = x + width;
	scissorsRect_.maxY = y + height;
}

bool D3E::RmlUi_NVRHI::LoadTexture(Rml::TextureHandle& texture_handle,
                                   Rml::Vector2i& texture_dimensions,
                                   const Rml::String& source)
{
	// Currently everything is static loaded, so no load action required
	if (!TextureFactory::IsTextureUuidValid(source.c_str()))
	{
		return false;
	}
	texture_handle = GetNewTextureHandle();
	takenTextureHandles_.insert(texture_handle);
	textureMap_.insert({texture_handle, TextureFactory::GetTextureHandle(source.c_str())});
	textureUuids_.insert({texture_handle, source.c_str()});
	bindingsCache_.insert({texture_handle, GetBindingSetHandle(texture_handle)});
	return true;
}

bool D3E::RmlUi_NVRHI::GenerateTexture(Rml::TextureHandle& texture_handle,
                                       const Rml::byte* source,
                                       const Rml::Vector2i& source_dimensions)
{
	nvrhi::TextureDesc textureDesc = {};
	textureDesc.setDimension(nvrhi::TextureDimension::Texture2D);
	textureDesc.setKeepInitialState( true );
	textureDesc.setInitialState(nvrhi::ResourceStates::ShaderResource);
	textureDesc.setWidth(source_dimensions.x);
	textureDesc.setHeight(source_dimensions.y);
	textureDesc.setFormat(nvrhi::Format::RGBA8_UNORM);

	texture_handle = GetNewTextureHandle();
	takenTextureHandles_.insert(texture_handle);
	textureMap_[texture_handle] = renderer_->createTexture(textureDesc);
	bindingsCache_.insert({texture_handle, GetBindingSetHandle(texture_handle)});

	commandList_->open();
	commandList_->writeTexture(textureMap_[texture_handle], 0, 0, source, sizeof(Rml::byte) * 4 * source_dimensions.x);
	commandList_->close();
	renderer_->executeCommandList(commandList_);

	if (textureMap_[texture_handle])
	{
		return true;
	}
	return false;
}

void D3E::RmlUi_NVRHI::ReleaseTexture(Rml::TextureHandle texture)
{
	if (takenTextureHandles_.find(texture) != takenTextureHandles_.end())
	{
		takenTextureHandles_.erase(texture);
	}
	if (textureUuids_.find(texture) != textureUuids_.end())
	{
		textureUuids_.erase(texture);
	}
	if (bindingsCache_.find(texture) != bindingsCache_.end())
	{
		bindingsCache_.erase(texture);
	}
	if (textureMap_.find(texture) != textureMap_.end())
	{
		textureMap_.erase(texture);
	}
}

void D3E::RmlUi_NVRHI::SetTransform(const Rml::Matrix4f* transform)
{
	auto row1 = transform->GetRow(0);
	auto row2 = transform->GetRow(1);
	auto row3 = transform->GetRow(2);
	auto row4 = transform->GetRow(3);

	cbData.gTransform._11 = row1[0];
	cbData.gTransform._12 = row1[1];
	cbData.gTransform._13 = row1[2];
	cbData.gTransform._14 = row1[3];

	cbData.gTransform._21 = row2[0];
	cbData.gTransform._22 = row2[1];
	cbData.gTransform._23 = row2[2];
	cbData.gTransform._24 = row2[3];

	cbData.gTransform._31 = row3[0];
	cbData.gTransform._32 = row3[1];
	cbData.gTransform._33 = row3[2];
	cbData.gTransform._34 = row3[3];

	cbData.gTransform._41 = row4[0];
	cbData.gTransform._42 = row4[1];
	cbData.gTransform._43 = row4[2];
	cbData.gTransform._44 = row4[3];

	commandList_->open();
	commandList_->writeBuffer(constantBuffer_, &cbData, sizeof(RmlCB));
	commandList_->close();
	renderer_->executeCommandList(commandList_);
}

D3E::RmlUi_NVRHI::RmlUi_NVRHI(Game* game, nvrhi::IFramebuffer* fb) : game_(game)
{
	renderer_ = game_->GetRender()->GetDevice();
	commandList_ = game_->GetRender()->GetCommandList();

	ShaderFactory::AddVertexShader("RmlUi", "RmlUi.hlsl", "VSMain");

	nvrhi::VertexAttributeDesc attributes[] = {
		nvrhi::VertexAttributeDesc()
			.setName("POSITION")
			.setFormat(nvrhi::Format::RGBA32_FLOAT)
			.setOffset(offsetof(RmlVertex, pos))
			.setElementStride(sizeof(RmlVertex)),
		nvrhi::VertexAttributeDesc()
			.setName("COLOR")
			.setFormat(nvrhi::Format::RGBA32_FLOAT)
			.setOffset(offsetof(RmlVertex, col))
			.setElementStride(sizeof(RmlVertex)),
		nvrhi::VertexAttributeDesc()
			.setName("TEXCOORD")
			.setFormat(nvrhi::Format::RGBA32_FLOAT)
			.setOffset(offsetof(RmlVertex, tex))
			.setElementStride(sizeof(RmlVertex)),
	};
	ShaderFactory::AddInputLayout("RmlUi", attributes, 3,
		                              ShaderFactory::GetVertexShader("RmlUi"));

	ShaderFactory::AddPixelShader("RmlUiCol", "RmlUiCol.hlsl", "PSMain");
	ShaderFactory::AddPixelShader("RmlUiTex", "RmlUi.hlsl", "PSMain");

	auto constantBufferDesc = nvrhi::BufferDesc();
	constantBufferDesc.setByteSize(sizeof(RmlCB));
	constantBufferDesc.setIsConstantBuffer(true);
	constantBufferDesc.setIsVolatile(false);
	constantBufferDesc.setMaxVersions(16);
	constantBufferDesc.setKeepInitialState(true);
	constantBuffer_ = renderer_->createBuffer(constantBufferDesc);

	nvrhi::BindingLayoutDesc layoutDescV = {};
	layoutDescV.setVisibility(nvrhi::ShaderType::Vertex);
	layoutDescV.addItem(nvrhi::BindingLayoutItem::ConstantBuffer(0));
	ShaderFactory::AddBindingLayout("RmlUiV", layoutDescV);

	nvrhi::BindingSetDesc secDescV = {};
	secDescV.addItem(nvrhi::BindingSetItem::ConstantBuffer(0, constantBuffer_));
	bindingSetV_ = renderer_->createBindingSet(secDescV, ShaderFactory::GetBindingLayout("RmlUiV"));

	nvrhi::BindingLayoutDesc layoutDescColP = {};
	layoutDescColP.setVisibility(nvrhi::ShaderType::Pixel);
	ShaderFactory::AddBindingLayout("RmlUiColP", layoutDescColP);

	nvrhi::BindingSetDesc secDescColP = {};
	nullBindingSetP_ = renderer_->createBindingSet(secDescColP, ShaderFactory::GetBindingLayout("RmlUiColP"));

	nvrhi::BindingLayoutDesc layoutDescTexP = {};
	layoutDescTexP.setVisibility(nvrhi::ShaderType::Pixel);
	layoutDescTexP.addItem(nvrhi::BindingLayoutItem::Texture_SRV(0));
	layoutDescTexP.addItem(nvrhi::BindingLayoutItem::Sampler(0));
	ShaderFactory::AddBindingLayout("RmlUiTexP", layoutDescTexP);
	bindingLayout_ = ShaderFactory::GetBindingLayout("RmlUiTexP");

	nvrhi::DepthStencilState depthStencilState = {};
	depthStencilState.setDepthTestEnable(false);
	depthStencilState.setDepthWriteEnable(false);
	depthStencilState.setStencilEnable(false);

	nvrhi::RasterState rasterState = {};
	rasterState.fillMode = nvrhi::RasterFillMode::Solid;
	rasterState.scissorEnable = true;
	rasterState.depthClipEnable = false;
	rasterState.setCullNone();

	nvrhi::BlendState blendState = {};
	blendState.targets[0].blendEnable = true;
	blendState.targets[0].destBlend = nvrhi::BlendFactor::OneMinusSrcAlpha;
	blendState.targets[0].srcBlend = nvrhi::BlendFactor::SrcAlpha;
	blendState.targets[0].destBlendAlpha = nvrhi::BlendFactor::One;
	blendState.targets[0].srcBlendAlpha = nvrhi::BlendFactor::Zero;
	blendState.targets[0].blendOp = nvrhi::BlendOp::Add;

	nvrhi::RenderState renderState = {};
	renderState.depthStencilState = depthStencilState;
	renderState.rasterState = rasterState;
	renderState.blendState = blendState;

	nvrhi::GraphicsPipelineDesc pipelineColDesc = {};
	pipelineColDesc.setInputLayout(ShaderFactory::GetInputLayout("RmlUi"));
	pipelineColDesc.setVertexShader(ShaderFactory::GetVertexShader("RmlUi"));
	pipelineColDesc.setPixelShader(ShaderFactory::GetPixelShader("RmlUiCol"));
	pipelineColDesc.addBindingLayout(ShaderFactory::GetBindingLayout("RmlUiV"));
	pipelineColDesc.addBindingLayout(ShaderFactory::GetBindingLayout("RmlUiColP"));
	pipelineColDesc.setRenderState(renderState);
	pipelineColDesc.primType = nvrhi::PrimitiveType::TriangleList;
	ShaderFactory::AddGraphicsPipeline("RmlUiCol", pipelineColDesc, fb);

	nvrhi::GraphicsPipelineDesc pipelineTexDesc = {};
	pipelineTexDesc.setInputLayout(ShaderFactory::GetInputLayout("RmlUi"));
	pipelineTexDesc.setVertexShader(ShaderFactory::GetVertexShader("RmlUi"));
	pipelineTexDesc.setPixelShader(ShaderFactory::GetPixelShader("RmlUiTex"));
	pipelineTexDesc.addBindingLayout(ShaderFactory::GetBindingLayout("RmlUiV"));
	pipelineTexDesc.addBindingLayout(ShaderFactory::GetBindingLayout("RmlUiTexP"));
	pipelineTexDesc.setRenderState(renderState);
	pipelineTexDesc.primType = nvrhi::PrimitiveType::TriangleList;
	ShaderFactory::AddGraphicsPipeline("RmlUiTex", pipelineTexDesc, fb);

	psoCol_ = ShaderFactory::GetGraphicsPipeline("RmlUiCol");
	psoTex_ = ShaderFactory::GetGraphicsPipeline("RmlUiTex");

	viewportState_ = {};
	viewportState_.addViewport(nvrhi::Viewport(EngineState::GetGameViewportWidth(), EngineState::GetGameViewportHeight()));
	viewportState_.scissorRects = { nvrhi::Rect(0, EngineState::GetGameViewportWidth(), 0, EngineState::GetGameViewportHeight()) };
	graphicsState_.setViewport(viewportState_);

	auto samplerDesc = nvrhi::SamplerDesc();
	samplerDesc.minFilter = true;
	samplerDesc.magFilter = true;
	samplerDesc.mipFilter = true;
	samplerDesc.addressU = nvrhi::SamplerAddressMode::Clamp;
	samplerDesc.addressV = nvrhi::SamplerAddressMode::Clamp;
	samplerDesc.addressW = nvrhi::SamplerAddressMode::Clamp;
	sampler_ = renderer_->createSampler(samplerDesc);
}

D3E::RmlUi_NVRHI::~RmlUi_NVRHI()
{
}

nvrhi::BindingSetHandle& D3E::RmlUi_NVRHI::GetBindingSetHandle(const Rml::TextureHandle& texture)
{
	auto iter = bindingsCache_.find(texture);
	if (iter != bindingsCache_.end())
	{
		return iter->second;
	}

	nvrhi::BindingSetDesc desc;

	desc.bindings = {
		nvrhi::BindingSetItem::Texture_SRV(0, textureMap_[texture]),
		nvrhi::BindingSetItem::Sampler(0, sampler_)
	};

	nvrhi::BindingSetHandle binding;
	binding = renderer_->createBindingSet(desc, bindingLayout_);
	assert(binding);

	bindingsCache_[texture] = binding;
	return bindingsCache_[texture];
}

Rml::TextureHandle D3E::RmlUi_NVRHI::GetNewTextureHandle()
{
	for (uintptr_t i = 1; i <= INT64_MAX; ++i)
	{
		if (takenTextureHandles_.find(i) == takenTextureHandles_.end())
		{
			return (Rml::TextureHandle)i;
		}
	}
	return 0;
}

bool D3E::RmlUi_NVRHI::ReallocateBuffer(nvrhi::BufferHandle& buffer,
                                        size_t requiredSize,
                                        size_t reallocateSize,
                                        bool isIndexBuffer)
{
	if (buffer == nullptr || size_t(buffer->getDesc().byteSize) < requiredSize)
	{
		nvrhi::BufferDesc desc;
		desc.byteSize = uint32_t(reallocateSize);
		desc.structStride = 0;
		desc.debugName = isIndexBuffer ? "RmlUi index buffer" : "RmlUi vertex buffer";
		desc.canHaveUAVs = false;
		desc.isVertexBuffer = !isIndexBuffer;
		desc.isIndexBuffer = isIndexBuffer;
		desc.isDrawIndirectArgs = false;
		desc.isVolatile = false;
		desc.initialState = isIndexBuffer ? nvrhi::ResourceStates::IndexBuffer : nvrhi::ResourceStates::VertexBuffer;
		desc.keepInitialState = true;

		buffer = renderer_->createBuffer(desc);

		if (!buffer)
		{
			return false;
		}
	}

	return true;
}

Rml::CompiledGeometryHandle D3E::RmlUi_NVRHI::GetNewCompiledGeometryHandle()
{
	for (uintptr_t i = 1; i <= INT64_MAX; ++i)
	{
		if (takenCompiledGeometryHandles_.find(i) == takenCompiledGeometryHandles_.end())
		{
			return (Rml::CompiledGeometryHandle)i;
		}
	}
	return 0;
}
