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
#include "engine/components/ObjectInfoComponent.h"

void D3E::StaticMeshRenderSystem::Draw(entt::registry& reg, nvrhi::IFramebuffer* fb,
                                         nvrhi::ICommandList* commandList, nvrhi::IDevice* device)
{
	Vector3 origin = {0, 0, 0};

	auto playerView = reg.view<const TransformComponent, const CameraComponent, const FPSControllerComponent>();

	CameraComponent cameraCopy; // TODO: this is terrible, redisign camerautils to avoid

	for(auto [entity, tc, cc, fpscc] : playerView.each())
	{
		origin.x = tc.position_.x + cc.offset.x;
		origin.y = tc.position_.y + cc.offset.y;
		origin.z = tc.position_.z + cc.offset.z;
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

				  DirectX::SimpleMath::Vector3 translation = {tc.position_.x, tc.position_.y, tc.position_.z};
				  DirectX::SimpleMath::Quaternion rotation = {tc.rotation_.x, tc.rotation_.y, tc.rotation_.z, tc.rotation_.w};
				  DirectX::SimpleMath::Vector3 scale = {tc.scale_.x, tc.scale_.y, tc.scale_.z};

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
		                                   .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(640, 480)))
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
