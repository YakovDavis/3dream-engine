#include "StaticMeshRenderSystem.h"

#include "D3E/CommonCpp.h"
#include "D3E/Components/FPSControllerComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Debug.h"
#include "D3E/components/TransformComponent.h"
#include "D3E/components/render/StaticMeshComponent.h"
#include "assetmng/MeshFactory.h"
#include "render/CameraUtils.h"
#include "render/PerObjectConstBuffer.h"
#include "render/ShaderFactory.h"

void D3E::StaticMeshRenderSystem::Draw(entt::registry& reg, nvrhi::IFramebuffer* fb,
                                         nvrhi::ICommandList* commandList, nvrhi::IDevice* device)
{
	eastl::fixed_vector<float, 3, false> origin = {0, 0, 0};

	auto playerView = reg.view<const TransformComponent, const CameraComponent, const FPSControllerComponent>();

	CameraComponent cameraCopy; // TODO: this is terrible, redisign camerautils to avoid

	for(auto [entity, tc, cc, fpscc] : playerView.each())
	{
		origin[0] = tc.position_[0] + cc.offset[0];
		origin[1] = tc.position_[1] + cc.offset[1];
		origin[2] = tc.position_[2] + cc.offset[2];
		cameraCopy = cc;
		break;
	}

	auto view = reg.view<const TransformComponent, const StaticMeshComponent>();

	view.each([commandList, fb, origin, cameraCopy](const auto& tc, const auto& smc)
	          {
				  if (!smc.initialized)
				  {
					  return;
				  }
				  // Fill the constant buffer
				  PerObjectConstBuffer constBufferData = {};

				  DirectX::SimpleMath::Vector3 translation = {tc.position_[0], tc.position_[1], tc.position_[2]};
				  DirectX::SimpleMath::Quaternion rotation = {tc.rotation_[0], tc.rotation_[1], tc.rotation_[2], tc.rotation_[3]};
				  DirectX::SimpleMath::Vector3 scale = {tc.scale_[0], tc.scale_[1], tc.scale_[2]};

				  const DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(scale) * DirectX::SimpleMath::Matrix::CreateTranslation(translation);

				  constBufferData.gWorldViewProj = world * CameraUtils::GetViewProj(origin, cameraCopy);
				  constBufferData.gWorld = world;
				  constBufferData.gWorldView = world * CameraUtils::GetView(origin, cameraCopy);
				  constBufferData.gInvTrWorldView = (DirectX::SimpleMath::Matrix::CreateScale(1)).Invert().Transpose() * CameraUtils::GetViewProj(origin, cameraCopy);

				  commandList->writeBuffer(smc.constantBuffer, &constBufferData, sizeof(constBufferData));


				  // Set the graphics state: pipeline, framebuffer, viewport, bindings.
				  auto graphicsState = nvrhi::GraphicsState()
		                                   .setPipeline(ShaderFactory::GetGraphicsPipeline(smc.pipelineName))
		                                   .setFramebuffer(fb)
		                                   .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(1920, 1080))) // TODO: Un-hardcode!
		                                   .addBindingSet(ShaderFactory::GetBindingSet("SimpleForwardV"))
		                                   .addBindingSet(ShaderFactory::GetBindingSet("SimpleForwardP"))
		                                   .addVertexBuffer(MeshFactory::GetVertexBufferBinding(smc.meshName));
				  graphicsState.setIndexBuffer(MeshFactory::GetIndexBufferBinding(smc.meshName));
				  commandList->setGraphicsState(graphicsState);

				  // Draw our geometry
				  auto drawArguments = nvrhi::DrawArguments()
		                                   .setVertexCount(MeshFactory::GetMeshData(smc.meshName).indices.size());
				  commandList->drawIndexed(drawArguments);
			  });
}
