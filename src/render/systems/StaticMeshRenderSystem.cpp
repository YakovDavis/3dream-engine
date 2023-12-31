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
#include "render/CameraUtils.h"
#include "render/PerObjectConstBuffer.h"
#include "render/ShaderFactory.h"

void D3E::StaticMeshRenderSystem::Draw(entt::registry& reg, nvrhi::IFramebuffer* fb,
                                         nvrhi::ICommandList* commandList, nvrhi::IDevice* device)
{
	Vector3 origin = {0, 0, 0};

	auto playerView = reg.view<const TransformComponent, const CameraComponent>();

	const CameraComponent* camera = nullptr;

	for(auto [entity, tc, cc] : playerView.each())
	{
		origin = tc.position + cc.offset;
		camera = &cc;
		break;
	}

	if (!camera)
	{
		Debug::LogWarning("[StaticMeshRenderSystem] Camera not found");
		return;
	}

	auto view = reg.view<const ObjectInfoComponent, const TransformComponent, const StaticMeshComponent>();

	view.each([commandList, fb, origin, camera](const auto& info, const auto& tc, const auto& smc)
	          {
				  if (!smc.initialized)
				  {
					  Debug::LogError("sm not initialized");
					  return;
				  }
				  // Fill the constant buffer
				  PerObjectConstBuffer constBufferData = {};

				  const DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(tc.scale) * DirectX::SimpleMath::Matrix::CreateFromQuaternion(tc.rotation) * DirectX::SimpleMath::Matrix::CreateTranslation(tc.position);

				  constBufferData.gWorldViewProj = world * CameraUtils::GetViewProj(origin, *camera);
				  constBufferData.gWorld = world;
				  constBufferData.gWorldView = world * CameraUtils::GetView(origin, *camera);
				  constBufferData.gInvTrWorldView = (world * CameraUtils::GetView(origin, *camera)).Invert().Transpose();

				  commandList->writeBuffer(smc.constantBuffer, &constBufferData, sizeof(constBufferData));

				  auto renderModeCVar = ConsoleManager::getInstance()->findConsoleVariable("renderingMode");

				  // Set the graphics state: pipeline, framebuffer, viewport, bindings.
				  auto graphicsState = nvrhi::GraphicsState()
		                                   .setPipeline(renderModeCVar->getInt() == 0 ? ShaderFactory::GetGraphicsPipeline(smc.pipelineName) : ShaderFactory::GetGraphicsPipeline("WireFrame"))
		                                   .setFramebuffer(fb)
		                                   .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(1920, 1080)))
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
