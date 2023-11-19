#include "StaticMeshRenderSystem.h"

#include "D3E/CommonCpp.h"
#include "D3E/Components/FPSControllerComponent.h"
#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Debug.h"
#include "D3E/components/TransformComponent.h"
#include "D3E/components/render/StaticMeshComponent.h"
#include "assetmng/MeshFactory.h"
#include "render/CameraUtils.h"
#include "render/PerObjectConstBuffer.h"
#include "render/ShaderFactory.h"

#include <iostream>

void D3E::StaticMeshRenderSystem::Draw(entt::registry& reg, nvrhi::IFramebuffer* fb,
                                         nvrhi::ICommandList* commandList, nvrhi::IDevice* device)
{
	Vector3 origin = {0, 0, 0};

	auto playerView = reg.view<const TransformComponent, const CameraComponent, const FPSControllerComponent>();

	CameraComponent cameraCopy; // TODO: this is terrible, redesign camerautils to avoid

	for(auto [entity, tc, cc, fpscc] : playerView.each())
	{
		origin = tc.position + cc.offset;
		cameraCopy = cc;
		break;
	}

	auto view = reg.view<const ObjectInfoComponent, const TransformComponent, const StaticMeshComponent>();

	view.each([commandList, fb, origin, cameraCopy](const auto& info, const auto& tc, const auto& smc)
	          {
				  if (!smc.initialized)
				  {
					  Debug::LogError("sm not initialized");
					  return;
				  }
				  // Fill the constant buffer
				  PerObjectConstBuffer constBufferData = {};

				  const DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(tc.scale) * DirectX::SimpleMath::Matrix::CreateFromQuaternion(tc.rotation) * DirectX::SimpleMath::Matrix::CreateTranslation(tc.position);

				  constBufferData.gWorldViewProj = world * CameraUtils::GetViewProj(origin, cameraCopy);
				  constBufferData.gWorld = world;
				  constBufferData.gWorldView = world * CameraUtils::GetView(origin, cameraCopy);
				  constBufferData.gInvTrWorldView = (DirectX::SimpleMath::Matrix::CreateScale(tc.scale) * DirectX::SimpleMath::Matrix::CreateFromQuaternion(tc.rotation)).Invert().Transpose() * CameraUtils::GetViewProj(origin, cameraCopy);

				  commandList->writeBuffer(smc.constantBuffer, &constBufferData, sizeof(constBufferData));


				  // Set the graphics state: pipeline, framebuffer, viewport, bindings.
				  auto graphicsState = nvrhi::GraphicsState()
		                                   .setPipeline(ShaderFactory::GetGraphicsPipeline(smc.pipelineName))
		                                   .setFramebuffer(fb)
		                                   .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(1920, 1080)))
		                                   .addBindingSet(ShaderFactory::GetBindingSet(info.name + "V"))
		                                   .addBindingSet(ShaderFactory::GetBindingSet(info.name + "P"))
		                                   .addVertexBuffer(MeshFactory::GetVertexBufferBinding(smc.meshName));
				  graphicsState.setIndexBuffer(MeshFactory::GetIndexBufferBinding(smc.meshName));
				  commandList->setGraphicsState(graphicsState);

				  // Draw our geometry
				  auto drawArguments = nvrhi::DrawArguments()
		                                   .setVertexCount(MeshFactory::GetMeshData(smc.meshName).indices.size());
				  commandList->drawIndexed(drawArguments);
			  });
}
