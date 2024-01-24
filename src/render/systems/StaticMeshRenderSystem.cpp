#include "StaticMeshRenderSystem.h"

#include "D3E/CommonCpp.h"
#include "D3E/Components/FPSControllerComponent.h"
#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Debug.h"
#include "D3E/components/TransformComponent.h"
#include "D3E/components/render/StaticMeshComponent.h"
#include "D3E/engine/ConsoleManager.h"
#include "assetmng/MeshFactory.h"
#include "core/EngineState.h"
#include "render/CameraUtils.h"
#include "render/PerObjectConstBuffer.h"
#include "render/RenderUtils.h"
#include "render/DebugRenderer.h"
#include "render/ShaderFactory.h"

void D3E::StaticMeshRenderSystem::Draw(entt::registry& reg, nvrhi::IFramebuffer* fb,
                                         nvrhi::ICommandList* commandList, nvrhi::IDevice* device)
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

	auto view = reg.view<const ObjectInfoComponent, const TransformComponent, const StaticMeshComponent>();

	view.each([commandList, fb, origin, camera](const auto& info, const auto& tc, const auto& smc)
	          {
				  if (!smc.initialized)
				  {
					  Debug::LogError("sm not initialized");
					  return;
				  }

				  auto view = CameraUtils::GetView(origin, *camera);

				  const DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(tc.scale) * DirectX::SimpleMath::Matrix::CreateFromQuaternion(tc.rotation) * DirectX::SimpleMath::Matrix::CreateTranslation(tc.position);

				  // frustum culling
				  DirectX::BoundingFrustum boundingFrustum;
				  camera->boundingFrustum.Transform(boundingFrustum, view.Invert());
				  DirectX::BoundingBox boundingBox = MeshFactory::GetMeshData(smc.meshUuid).boundingBox;
				  boundingBox.Transform(boundingBox, world);

				  if (!boundingFrustum.Intersects(boundingBox))
				  {
					  return;
				  }

				  if (ConsoleManager::getInstance()->findConsoleVariable("visualizeBounds")->getInt() > 0)
				  {
					  if (auto debugRenderer = RenderUtils::GetDebugRenderer())
					  {
						  debugRenderer->QueueAxisAlignedBox(boundingBox.Center, boundingBox.Extents, Color(1, 1, 1, 1));
					  }
				  }

				  // Fill the constant buffer
				  PerObjectConstBuffer constBufferData = {};

				  constBufferData.gWorldViewProj = world * CameraUtils::GetViewProj(origin, *camera);
				  constBufferData.gWorld = world;
				  constBufferData.gWorldView = world * view;
				  constBufferData.gInvTrRotation = DirectX::SimpleMath::Matrix::CreateFromQuaternion(tc.rotation).Invert().Transpose();
				  constBufferData.gEditorId = info.editorId;

				  commandList->writeBuffer(smc.constantBuffer, &constBufferData, sizeof(constBufferData));

				  const auto& defaultPSO = smc.editorHighlighted ? ShaderFactory::GetGraphicsPipeline(smc.pipelineName + "Highlight") : ShaderFactory::GetGraphicsPipeline(smc.pipelineName);

				  auto renderModeCVar = ConsoleManager::getInstance()->findConsoleVariable("renderingMode");

				  // Set the graphics state: pipeline, framebuffer, viewport, bindings.
				  auto graphicsState = nvrhi::GraphicsState()
		                                   .setPipeline(renderModeCVar->getInt() == 0 ? defaultPSO : ShaderFactory::GetGraphicsPipeline("WireFrame"))
		                                   .setFramebuffer(fb)
		                                   .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(EngineState::GetViewportWidth(), EngineState::GetViewportHeight())))
		                                   .addBindingSet(ShaderFactory::GetBindingSetV(info.id))
		                                   .addBindingSet(ShaderFactory::GetBindingSetP(info.id))
		                                   .addVertexBuffer(MeshFactory::GetVertexBufferBinding(smc.meshUuid));
				  graphicsState.setIndexBuffer(MeshFactory::GetIndexBufferBinding(smc.meshUuid));
				  commandList->setGraphicsState(graphicsState);

				  // Draw our geometry
				  auto drawArguments = nvrhi::DrawArguments()
		                                   .setVertexCount(MeshFactory::GetMeshData(smc.meshUuid).indices.size());
				  commandList->drawIndexed(drawArguments);
			  });
}
