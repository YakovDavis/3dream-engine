#include "Csm.h"

#include "CameraUtils.h"
#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Debug.h"
#include "D3E/Game.h"
#include "D3E/components/TransformComponent.h"
#include "D3E/components/render/StaticMeshComponent.h"
#include "ShaderFactory.h"
#include "assetmng/MeshFactory.h"
#include "core/EngineState.h"
#include "render/GameRender.h"

D3E::Csm* D3E::Csm::instance_ = nullptr;

const int kCsmRes = 2048;

D3E::Csm::Csm(D3E::Game* game) :game_(game)
{
	shadowCascadeLevels_.push_back(1000.0f / 50.0f);
	shadowCascadeLevels_.push_back(1000.0f / 25.0f);
	shadowCascadeLevels_.push_back(1000.0f / 10.0f);
	shadowCascadeLevels_.push_back(1000.0f / 2.0f);

	device_ = game_->GetRender()->GetDevice();
	commandList_ = game_->GetRender()->GetCommandList();

	nvrhi::TextureDesc depthDesc = {};
	depthDesc.setDimension(nvrhi::TextureDimension::Texture2DArray);
	depthDesc.setWidth(kCsmRes);
	depthDesc.setHeight(kCsmRes);
	depthDesc.setArraySize(CASCADE_COUNT + 1);
	depthDesc.setMipLevels(1);
	depthDesc.setClearValue(1.0f);
	depthDesc.setFormat(nvrhi::Format::D32);
	depthDesc.setInitialState(nvrhi::ResourceStates::ShaderResource);
	depthDesc.setKeepInitialState(true);
	depthDesc.setIsRenderTarget(true);
	depthDesc.setIsTypeless(true);
	depthDesc.setDebugName("Csm Texture");
	csmTexture_ = device_->createTexture(depthDesc);

	nvrhi::BufferDesc vsCBDesc = {};
	vsCBDesc.setByteSize(sizeof(PerObjectConstBuffer));
	vsCBDesc.setIsConstantBuffer(true);
	vsCBDesc.setIsVolatile(false);
	vsCBDesc.setMaxVersions(16);
	vsCBDesc.setKeepInitialState(true);
	vsCB_ = device_->createBuffer(vsCBDesc);

	nvrhi::BufferDesc gsCBDesc = {};
	gsCBDesc.setByteSize(sizeof(DirectX::SimpleMath::Matrix) * (CASCADE_COUNT + 1) + sizeof(DirectX::SimpleMath::Vector4));
	gsCBDesc.setIsConstantBuffer(true);
	gsCBDesc.setIsVolatile(false);
	gsCBDesc.setMaxVersions(16);
	gsCBDesc.setKeepInitialState(true);
	gsCB_ = device_->createBuffer(gsCBDesc);

	nvrhi::BindingSetDesc vBindingSetDesc = {};
	vBindingSetDesc.addItem(nvrhi::BindingSetItem::ConstantBuffer(0, vsCB_));
	vsBS_ = device_->createBindingSet(vBindingSetDesc, ShaderFactory::GetBindingLayout("CsmV"));

	nvrhi::BindingSetDesc gBindingSetDesc = {};
	gBindingSetDesc.addItem(nvrhi::BindingSetItem::ConstantBuffer(0, gsCB_));
	gsBS_ = device_->createBindingSet(gBindingSetDesc, ShaderFactory::GetBindingLayout("CsmG"));

	nvrhi::FramebufferAttachment depthAttachmentDesc = {};
	depthAttachmentDesc.setTexture(csmTexture_);
	depthAttachmentDesc.setSubresources(nvrhi::AllSubresources);
	depthAttachmentDesc.setArraySliceRange(0, 5);
	depthAttachmentDesc.setMipLevel(0);
	depthAttachmentDesc.setFormat(nvrhi::Format::D32);
	nvrhi::FramebufferDesc framebufferDesc = {};
	framebufferDesc.colorAttachments = {};
	framebufferDesc.depthAttachment = depthAttachmentDesc;
	fb_ = device_->createFramebuffer(framebufferDesc);

	nvrhi::DepthStencilState depthStencilState = {};
	depthStencilState.setDepthTestEnable(true);
	depthStencilState.setDepthWriteEnable(true);
	depthStencilState.setStencilEnable(false);

	nvrhi::RasterState rasterState = {};
	rasterState.fillMode = nvrhi::RasterFillMode::Solid;
	rasterState.setCullNone();
	rasterState.setDepthClipEnable(true);

	nvrhi::BlendState blendState = {};
	blendState.targets[0] = {};

	nvrhi::RenderState renderState = {};
	renderState.depthStencilState = depthStencilState;
	renderState.rasterState = rasterState;
	renderState.blendState = blendState;

	nvrhi::GraphicsPipelineDesc pipelineDesc = {};
	pipelineDesc.setInputLayout(ShaderFactory::GetInputLayout("Csm"));
	pipelineDesc.setVertexShader(ShaderFactory::GetVertexShader("Csm"));
	pipelineDesc.setGeometryShader(ShaderFactory::GetGeometryShader("Csm"));
	pipelineDesc.setPixelShader(nullptr);
	pipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("CsmV"));
	pipelineDesc.addBindingLayout(ShaderFactory::GetBindingLayout("CsmG"));
	pipelineDesc.setRenderState(renderState);
	pipelineDesc.primType = nvrhi::PrimitiveType::TriangleList;
	pso_ = device_->createGraphicsPipeline(pipelineDesc, fb_);

	graphicsState_ = {};
	graphicsState_.setPipeline(pso_);
	graphicsState_.setFramebuffer(fb_);
	graphicsState_.setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(kCsmRes, kCsmRes)));

	nvrhi::SamplerDesc samplerDesc = {};
	samplerDesc.setAllFilters(true);
	samplerDesc.setAllAddressModes(nvrhi::SamplerAddressMode::Border);
	samplerDesc.borderColor = 1.0f;

	csmSampler_ = device_->createSampler(samplerDesc);
}

void D3E::Csm::DestroyResources()
{
}

D3E::Csm* D3E::Csm::Init(D3E::Game* game)
{
	if (instance_ == nullptr)
	{
		instance_ = new Csm(game);
		Debug::LogMessage("[Csm] Init");
	}

	return instance_;
}

void D3E::Csm::GenerateCascadeMaps()
{
	commandList_->open();
	commandList_->beginMarker("CascadeShadowMaps");
	commandList_->beginTrackingTextureState(csmTexture_, nvrhi::AllSubresources, nvrhi::ResourceStates::ShaderResource);
	commandList_->setTextureState(csmTexture_, nvrhi::AllSubresources, nvrhi::ResourceStates::DepthWrite);
	commandList_->clearDepthStencilTexture(csmTexture_, nvrhi::AllSubresources, true, 1.0f, false, 0);

	{
		entt::entity dLight = entt::null;

		auto view = game_->GetRegistry().view<ObjectInfoComponent, TransformComponent, LightComponent>();

		for (auto light : view)
		{
			if (game_->GetRegistry().get<LightComponent>(light).lightType == LightType::Directional)
			{
				dLight = light;
				break;
			}
		}

		if (dLight == entt::null)
		{
			return;
		}

		if (EngineState::currentPlayer == entt::null)
		{
			return;
		}
		const TransformComponent* playerTransform = game_->GetRegistry().try_get<TransformComponent>(EngineState::currentPlayer);
		if (!playerTransform)
		{
			return;
		}
		const CameraComponent* camera = game_->GetRegistry().try_get<CameraComponent>(EngineState::currentPlayer);
		if (!camera)
		{
			return;
		}
		Vector3 origin = playerTransform->position + camera->offset;

		auto tmp1 = GetLightSpaceMatrices(game_->GetRegistry().get<LightComponent>(dLight), origin, *camera);
		for (int i = 0; i < CASCADE_COUNT + 1; ++i)
		{
			gsCBData_.gViewProj[i] = tmp1[i];
		}
		gsCBData_.gDistances = GetShadowCascadeDistances();
	}

	auto view = game_->GetRegistry().view<const ObjectInfoComponent, const TransformComponent, const StaticMeshComponent>();

	commandList_->writeBuffer(gsCB_, &gsCBData_, gsCB_->getDesc().byteSize);
	commandList_->close();
	device_->executeCommandList(commandList_);

	view.each([&](const auto& info, const auto& tc, const auto& smc)
	          {
				  if (!smc.initialized)
				  {
					  return;
				  }

				  if (!smc.castsShadow)
				  {
					  return;
				  }

				  vsCBData_.gWorld = DirectX::SimpleMath::Matrix::CreateScale(tc.scale) * DirectX::SimpleMath::Matrix::CreateFromQuaternion(tc.rotation) * DirectX::SimpleMath::Matrix::CreateTranslation(tc.position);

				  commandList_->open();
				  commandList_->writeBuffer(vsCB_, &vsCBData_, sizeof(PerObjectConstBuffer));

				  graphicsState_.bindings = {vsBS_, gsBS_};
				  graphicsState_.vertexBuffers = {MeshFactory::GetVertexBufferBinding(smc.meshUuid)};
				  graphicsState_.indexBuffer = MeshFactory::GetIndexBufferBinding(smc.meshUuid);
				  commandList_->setGraphicsState(graphicsState_);

				  // Draw our geometry
				  auto drawArguments = nvrhi::DrawArguments()
		                                   .setVertexCount(MeshFactory::GetMeshData(smc.meshUuid).indices.size());
				  commandList_->drawIndexed(drawArguments);

				  commandList_->close();
				  device_->executeCommandList(commandList_);
			  });

	commandList_->open();
	commandList_->setTextureState(csmTexture_, nvrhi::AllSubresources, nvrhi::ResourceStates::ShaderResource);
	commandList_->endMarker();
	commandList_->close();
	device_->executeCommandList(commandList_);
}

DirectX::SimpleMath::Matrix D3E::Csm::GetLightSpaceMatrix(const LightComponent& lightComponent, const DirectX::SimpleMath::Vector3& parentLoc, const CameraComponent& cameraComponent, float nearPlane, float farPlane)
{
	const auto proj = DirectX::XMMatrixPerspectiveFovLH(
		cameraComponent.fov, cameraComponent.aspectRatio, nearPlane,
		farPlane);
	const auto corners = CameraUtils::GetFrustumCornersWorldSpace(CameraUtils::GetView(parentLoc, cameraComponent), proj);

	auto center = Vector3::Zero;
	for (const auto& v : corners)
	{
		center.x += v.x;
		center.y += v.y;
		center.z += v.z;
	}
	center /= static_cast<float>(corners.size());

	Vector3 lightUp = lightComponent.direction;
	lightUp = lightUp.Cross(Vector3::Up);
	lightUp = lightUp.Cross(lightComponent.direction);
	lightUp.Normalize();
	const auto lightView = DirectX::XMMatrixLookAtLH(center, center + lightComponent.direction, lightUp);

	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::lowest();
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::lowest();
	float minZ = std::numeric_limits<float>::max();
	float maxZ = std::numeric_limits<float>::lowest();
	for (const auto& v : corners)
	{
		const auto trf = Vector4::Transform(v, lightView);
		minX = (std::min)(minX, trf.x);
		maxX = (std::max)(maxX, trf.x);
		minY = (std::min)(minY, trf.y);
		maxY = (std::max)(maxY, trf.y);
		minZ = (std::min)(minZ, trf.z);
		maxZ = (std::max)(maxZ, trf.z);
	}

	constexpr float zMult = 5.0f; // how much geometry to include from outside the view frustum
	minZ = (minZ < 0) ? minZ * zMult : minZ / zMult;
	maxZ = (maxZ < 0) ? maxZ / zMult : maxZ * zMult;

	const auto lightProjection = DirectX::XMMatrixOrthographicOffCenterLH(minX, maxX, minY, maxY, minZ, maxZ);

	return lightView * lightProjection;
}

eastl::vector<DirectX::SimpleMath::Matrix>
D3E::Csm::GetLightSpaceMatrices(const D3E::LightComponent& lightComponent,
                                const Vector3& parentLoc,
                                const D3E::CameraComponent& cameraComponent)
{
	eastl::vector<Matrix> ret;
	for (size_t i = 0; i < shadowCascadeLevels_.size() + 1; ++i)
	{
		if (i == 0)
		{
			ret.push_back(GetLightSpaceMatrix(lightComponent, parentLoc, cameraComponent, cameraComponent.nearPlane, shadowCascadeLevels_[i]));
		}
		else if (i < shadowCascadeLevels_.size())
		{
			ret.push_back(GetLightSpaceMatrix(lightComponent, parentLoc, cameraComponent, shadowCascadeLevels_[i - 1], shadowCascadeLevels_[i]));
		}
		else
		{
			ret.push_back(GetLightSpaceMatrix(lightComponent, parentLoc, cameraComponent, shadowCascadeLevels_[i - 1], cameraComponent.farPlane));
		}
	}
	return ret;
}

DirectX::SimpleMath::Vector4 D3E::Csm::GetShadowCascadeDistances() const
{
	return Vector4(shadowCascadeLevels_[0], shadowCascadeLevels_[1],shadowCascadeLevels_[2],shadowCascadeLevels_[3]);
}
