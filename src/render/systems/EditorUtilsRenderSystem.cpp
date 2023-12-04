#include "EditorUtilsRenderSystem.h"

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
#include "render/components/GridComponent.h"
#include "render/ShaderFactory.h"
#include "assetmng/DefaultAssetLoader.h"

void D3E::EditorUtilsRenderSystem::InitRender(nvrhi::ICommandList* commandList, nvrhi::IDevice* device)
{

}

void D3E::EditorUtilsRenderSystem::Draw(entt::registry& reg,
                                        nvrhi::IFramebuffer* fb,
                                        nvrhi::ICommandList* commandList,
                                        nvrhi::IDevice* device)
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
		Debug::LogWarning("[EditorUtilsRenderSystem] Camera not found");
		return;
	}

	auto view = reg.view<const GridComponent>();

	view.each([origin, camera, commandList, fb](const auto& gc)
	          {
				  // Fill the constant buffer
				  PerObjectConstBuffer constBufferData = {};

				  float x = 0; //floor(origin.x / gc.segmentSize) * gc.segmentSize;
				  float z = 0; //floor(origin.z / gc.segmentSize) * gc.segmentSize;

				  const DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(gc.segmentSize) * DirectX::SimpleMath::Matrix::CreateTranslation(x, 0, z);

				  constBufferData.gWorldViewProj = world * CameraUtils::GetViewProj(origin, *camera);

				  commandList->writeBuffer(DefaultAssetLoader::GetEditorGridCB(), &constBufferData, sizeof(constBufferData));

				  // Set the graphics state: pipeline, framebuffer, viewport, bindings.
				  auto graphicsState = nvrhi::GraphicsState()
		                                   .setPipeline(ShaderFactory::GetGraphicsPipeline("LineList"))
		                                   .setFramebuffer(fb)
		                                   .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(1920, 1080)))
		                                   .addBindingSet(ShaderFactory::GetBindingSetV(kDebugLineBindingSetUUID))
		                                   .addBindingSet(ShaderFactory::GetBindingSetP(kDebugLineBindingSetUUID))
		                                   .addVertexBuffer(MeshFactory::GetVertexBufferBinding(kGridUUID));
				  graphicsState.setIndexBuffer(MeshFactory::GetIndexBufferBinding(kGridUUID));
				  commandList->setGraphicsState(graphicsState);

				  // Draw our geometry
				  auto drawArguments = nvrhi::DrawArguments()
		                                   .setVertexCount(MeshFactory::GetMeshData(kGridUUID).indices.size());
				  commandList->drawIndexed(drawArguments);
			  });
}
