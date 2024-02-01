#include "LightRenderSystem.h"

#include "D3E/CommonCpp.h"
#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Components/render/LightComponent.h"
#include "D3E/Debug.h"
#include "D3E/components/TransformComponent.h"
#include "D3E/engine/ConsoleManager.h"
#include "SimpleMath.h"
#include "assetmng/MeshFactory.h"
#include "assetmng/TextureFactory.h"
#include "core/EngineState.h"
#include "render/CameraUtils.h"
#include "render/IblConstBuffer.h"
#include "render/LightConstBuffer.h"
#include "render/LightVolumeConstBuffer.h"
#include "render/PointLightConstBuffer.h"
#include "render/ShaderFactory.h"

using namespace DirectX::SimpleMath;

void D3E::LightRenderSystem::Draw(entt::registry& reg, nvrhi::IFramebuffer* fb,
                                  nvrhi::ICommandList* commandList,
                                  nvrhi::IDevice* device)
{
	if (EngineState::currentPlayer == entt::null)
	{
		return;
	}
	const TransformComponent* playerTransform = reg.try_get<TransformComponent>(EngineState::currentPlayer);
	if (!playerTransform)
	{
		return;
	}
	const CameraComponent* camera = reg.try_get<CameraComponent>(EngineState::currentPlayer);
	if (!camera)
	{
		return;
	}
	Vector3 origin = playerTransform->position + camera->offset;

	Matrix viewMatrix = CameraUtils::GetView(origin, *camera);
	Matrix viewProjMatrix = CameraUtils::GetViewProj(origin, *camera);

	commandList->beginMarker("LightPass");

	// IBL
	{
		if (!iblCBuffer_)
		{
			auto volumeCBufferDesc =
				nvrhi::BufferDesc()
					.setByteSize(sizeof(IblConstBuffer))
					.setIsConstantBuffer(true)
					.setIsVolatile(false)
					.setMaxVersions(16)
					.setKeepInitialState(true);
			iblCBuffer_ =
				device->createBuffer(volumeCBufferDesc);

			nvrhi::BindingSetDesc bindingSetDescV = {};
			iblBSV_ = device->createBindingSet(bindingSetDescV, ShaderFactory::GetBindingLayout("IblPassV"));

			nvrhi::BindingSetDesc bindingSetDescP = {};
			bindingSetDescP.addItem(
				nvrhi::BindingSetItem::ConstantBuffer(
					0, iblCBuffer_));
			bindingSetDescP.addItem(
				nvrhi::BindingSetItem::Texture_SRV(
					0, TextureFactory::GetGBuffer()->albedoBuffer));
			bindingSetDescP.addItem(
				nvrhi::BindingSetItem::Texture_SRV(
					1, TextureFactory::GetGBuffer()->positionBuffer));
			bindingSetDescP.addItem(
				nvrhi::BindingSetItem::Texture_SRV(
					2, TextureFactory::GetGBuffer()->normalBuffer));
			bindingSetDescP.addItem(
				nvrhi::BindingSetItem::Texture_SRV(
					3, TextureFactory::GetGBuffer()
						   ->metalRoughnessBuffer));
			bindingSetDescP.addItem(
				nvrhi::BindingSetItem::Texture_SRV(
					4, TextureFactory::GetTextureHandle(
						   kEnvTextureUUID)));
			bindingSetDescP.addItem(
				nvrhi::BindingSetItem::Texture_SRV(
					5, TextureFactory::GetTextureHandle(
						   kIrMapTextureUUID)));
			bindingSetDescP.addItem(
				nvrhi::BindingSetItem::Texture_SRV(
					6, TextureFactory::GetTextureHandle(
						   kSpBrdfLutTextureUUID)));
			bindingSetDescP.addItem(nvrhi::BindingSetItem::Sampler(
				0, TextureFactory::GetSampler("BaseGraphics")));
			bindingSetDescP.addItem(nvrhi::BindingSetItem::Sampler(
				1, TextureFactory::GetSampler("SpBrdf")));
			iblBSP_ = device->createBindingSet(bindingSetDescP, ShaderFactory::GetBindingLayout("IblPassP"));
		}

		IblConstBuffer constBufferData = {};
		constBufferData.gEyePosition =
			Vector4(origin.x, origin.y, origin.z, 1.0f);

		commandList->writeBuffer(iblCBuffer_,
		                         &constBufferData,
		                         sizeof(constBufferData));

		nvrhi::GraphicsState graphicsState = {};
		graphicsState.setPipeline(
			ShaderFactory::GetGraphicsPipeline("IblPass"));
		graphicsState.setFramebuffer(fb);
		graphicsState.setViewport(
			nvrhi::ViewportState().addViewportAndScissorRect(
				nvrhi::Viewport(
					EngineState::GetGameViewportWidth(),
					EngineState::GetGameViewportWidth())));
		graphicsState.addBindingSet(iblBSV_);
		graphicsState.addBindingSet(iblBSP_);
		commandList->setGraphicsState(graphicsState);

		auto drawArguments =
			nvrhi::DrawArguments().setVertexCount(4);
		commandList->draw(drawArguments);
	}

	auto view = reg.view<const ObjectInfoComponent, const TransformComponent, const LightComponent>();

	view.each([viewMatrix, viewProjMatrix, commandList, fb, origin](const auto& info, const auto& tc, const auto& lc)
	          {
				  if (lc.lightType == LightType::Directional)
				  {
					  LightConstBuffer constBufferData = {};

					  constBufferData.gEyePosition =
						  Vector4(origin.x, origin.y, origin.z, 1.0f);
					  constBufferData.gLightDir = Vector4(
						  lc.direction.x, lc.direction.y, lc.direction.z, 0.0f);
					  constBufferData.gLightDir.Normalize();
					  constBufferData.gLightColor =
						  Vector4(lc.color.x, lc.color.y, lc.color.z, 1.0f) * lc.intensity;
					  constBufferData.gLightColor.w = lc.castsShadows ? 1.0f : 0.0f;
					  constBufferData.gView = viewMatrix;

					  commandList->writeBuffer(lc.lightCBuffer,
			                                   &constBufferData,
			                                   sizeof(constBufferData));

					  nvrhi::GraphicsState graphicsState = {};
					  graphicsState.setPipeline(
						  ShaderFactory::GetGraphicsPipeline("LightPass"));
					  graphicsState.setFramebuffer(fb);
					  graphicsState.setViewport(
						  nvrhi::ViewportState().addViewportAndScissorRect(
							  nvrhi::Viewport(
								  EngineState::GetGameViewportWidth(),
								  EngineState::GetGameViewportWidth())));
					  graphicsState.addBindingSet(
						  ShaderFactory::GetBindingSetV(info.id));
					  graphicsState.addBindingSet(
						  ShaderFactory::GetBindingSetP(info.id));
					  commandList->setGraphicsState(graphicsState);

					  auto drawArguments =
						  nvrhi::DrawArguments().setVertexCount(4);
					  commandList->draw(drawArguments);
				  }
				  else if (lc.lightType == LightType::Point)
				  {
					  PointLightConstBuffer constBufferData = {};
					  constBufferData.gEyePosition =
						  Vector4(origin.x, origin.y, origin.z, 1.0f);
					  constBufferData.gLightPos = Vector4(
						  tc.position.x, tc.position.y, tc.position.z, 1.0f);
					  constBufferData.gLightColor =
						  Vector4(lc.color.x, lc.color.y, lc.color.z, 1.0f) * lc.intensity * 100; // 100 is arbitrary, just to make switching from directional to point less jarring

					  LightVolumeConstBuffer volumeConstBufferData = {};
					  volumeConstBufferData.gWorldViewProj = (Matrix::CreateScale(lc.attenuationLimitRadius, lc.attenuationLimitRadius, lc.attenuationLimitRadius) *
			                                                 Matrix::Identity * Matrix::CreateTranslation(tc.position)) * viewProjMatrix;

					  commandList->writeBuffer(lc.lightCBuffer,
			                                   &constBufferData,
			                                   sizeof(constBufferData));

					  commandList->writeBuffer(lc.volumeCBuffer,
			                                   &volumeConstBufferData,
			                                   sizeof(volumeConstBufferData));

					  nvrhi::GraphicsState graphicsState = {};
					  graphicsState.setPipeline(
						  ShaderFactory::GetGraphicsPipeline("PointLightPass"));
					  graphicsState.setFramebuffer(fb);
					  graphicsState.setViewport(
						  nvrhi::ViewportState().addViewportAndScissorRect(
							  nvrhi::Viewport(
								  EngineState::GetGameViewportWidth(),
								  EngineState::GetGameViewportWidth())));
					  graphicsState.addBindingSet(
						  ShaderFactory::GetBindingSetV(info.id));
					  graphicsState.addBindingSet(
						  ShaderFactory::GetBindingSetP(info.id));
					  graphicsState.addVertexBuffer(MeshFactory::GetVertexBufferBinding(kSphereUUID));
					  graphicsState.setIndexBuffer(MeshFactory::GetIndexBufferBinding(kSphereUUID));
					  commandList->setGraphicsState(graphicsState);

					  auto drawArguments =
						  nvrhi::DrawArguments().setVertexCount(MeshFactory::GetMeshData(kSphereUUID).indices.size());
					  commandList->drawIndexed(drawArguments);
				  }
			  });

	commandList->endMarker();
}
