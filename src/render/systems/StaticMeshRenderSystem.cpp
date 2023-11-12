#include "StaticMeshRenderSystem.h"

#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Debug.h"
#include "D3E/components/TransformComponent.h"
#include "D3E/components/render/StaticMeshComponent.h"
#include "assetmng/MeshFactory.h"
#include "render/CameraUtils.h"
#include "render/PerObjectConstBuffer.h"
#include "render/ShaderFactory.h"

void D3E::StaticMeshRenderSystem::Render(entt::registry& reg, nvrhi::IFramebuffer* fb,
                                         nvrhi::CommandListHandle& commandList)
{
	auto view = reg.view<const TransformComponent, const StaticMeshComponent>();

	view.each([commandList, fb](const auto& tc, const auto& smc)
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

				  CameraComponent cameraComponent;

				  eastl::fixed_vector<float, 3, false> origin = {0, 0, 0}; // player pos, TODO: need to acquire

				  constBufferData.gWorldViewProj = world * CameraUtils::GetViewProj(origin, cameraComponent);
				  constBufferData.gWorld = world;
				  constBufferData.gWorldView = world * CameraUtils::GetView(origin, cameraComponent);
				  constBufferData.gInvTrWorldView = (DirectX::SimpleMath::Matrix::CreateScale(1)).Invert().Transpose() * CameraUtils::GetViewProj(origin, cameraComponent);

				  commandList->writeBuffer(smc.constantBuffer, &constBufferData, sizeof(constBufferData));


				  // Set the graphics state: pipeline, framebuffer, viewport, bindings.
				  auto graphicsState = nvrhi::GraphicsState()
		                                   .setPipeline(ShaderFactory::GetGraphicsPipeline(smc.pipelineName))
		                                   .setFramebuffer(fb)
		                                   .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(640, 480)))
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
