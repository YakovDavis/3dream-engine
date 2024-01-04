#include "EditorUtilsRenderSystem.h"

#include "D3E/CommonCpp.h"
#include "D3E/Components/FPSControllerComponent.h"
#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Debug.h"
#include "D3E/Game.h"
#include "D3E/components/TransformComponent.h"
#include "D3E/components/render/StaticMeshComponent.h"
#include "D3E/engine/ConsoleManager.h"
#include "assetmng/DefaultAssetLoader.h"
#include "assetmng/MeshFactory.h"
#include "core/EngineState.h"
#include "input/InputDevice.h"
#include "render/CameraUtils.h"
#include "render/PerObjectConstBuffer.h"
#include "render/ShaderFactory.h"
#include "render/components/GridComponent.h"

bool D3E::EditorUtilsRenderSystem::isSelectionDirty = false;

void D3E::EditorUtilsRenderSystem::InitRender(nvrhi::ICommandList* commandList, nvrhi::IDevice* device)
{

}

void D3E::EditorUtilsRenderSystem::Draw(entt::registry& reg,
                                        nvrhi::IFramebuffer* fb,
                                        nvrhi::ICommandList* commandList,
                                        nvrhi::IDevice* device)
{
	nvrhi::GraphicsState graphicsState = {};
	graphicsState.setPipeline(ShaderFactory::GetGraphicsPipeline("EditorHighlightPass"));
	graphicsState.setFramebuffer(fb);
	graphicsState.setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(EngineState::GetViewportWidth(), EngineState::GetViewportHeight())));
	graphicsState.addBindingSet(ShaderFactory::GetBindingSetV("EditorHighlightPass"));
	graphicsState.addBindingSet(ShaderFactory::GetBindingSetP("EditorHighlightPass"));
	commandList->setGraphicsState(graphicsState);
	auto drawArguments = nvrhi::DrawArguments()
	                         .setVertexCount(4);
	commandList->draw(drawArguments);

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

	auto cvarDisplayGrid = ConsoleManager::getInstance()->findConsoleVariable("displayGrid");

	if (cvarDisplayGrid->getInt() > 0)
	{
		auto view = reg.view<const GridComponent>();

		view.each(
			[origin, camera, commandList, fb](const auto& gc)
			{
				// Fill the constant buffer
				PerObjectConstBuffer constBufferData = {};

				float x =
					0; // floor(origin.x / gc.segmentSize) * gc.segmentSize;
				float z =
					0; // floor(origin.z / gc.segmentSize) * gc.segmentSize;

				const DirectX::SimpleMath::Matrix world =
					DirectX::SimpleMath::Matrix::CreateScale(gc.segmentSize) *
					DirectX::SimpleMath::Matrix::CreateTranslation(x, 0, z);

				constBufferData.gWorldViewProj =
					world * CameraUtils::GetViewProj(origin, *camera);

				commandList->writeBuffer(DefaultAssetLoader::GetEditorGridCB(),
			                             &constBufferData,
			                             sizeof(constBufferData));

				// Set the graphics state: pipeline, framebuffer, viewport, bindings.
				auto graphicsState =
					nvrhi::GraphicsState()
						.setPipeline(
							ShaderFactory::GetGraphicsPipeline("LineList"))
						.setFramebuffer(fb)
						.setViewport(
							nvrhi::ViewportState().addViewportAndScissorRect(
								nvrhi::Viewport(EngineState::GetViewportWidth(), EngineState::GetViewportHeight())))
						.addBindingSet(ShaderFactory::GetBindingSetV(
							kDebugLineBindingSetUUID))
						.addBindingSet(ShaderFactory::GetBindingSetP(
							kDebugLineBindingSetUUID))
						.addVertexBuffer(
							MeshFactory::GetVertexBufferBinding(kGridUUID));
				graphicsState.setIndexBuffer(
					MeshFactory::GetIndexBufferBinding(kGridUUID));
				commandList->setGraphicsState(graphicsState);

				// Draw our geometry
				auto drawArguments = nvrhi::DrawArguments().setVertexCount(
					MeshFactory::GetMeshData(kGridUUID).indices.size());
				commandList->drawIndexed(drawArguments);
			});
	}

	auto cvarVisualizeBounds = ConsoleManager::getInstance()->findConsoleVariable("visualizeBounds");

	if (cvarVisualizeBounds->getInt() > 0)
	{
		auto view = reg.view<const TransformComponent, const StaticMeshComponent>();

		view.each(
			[origin, camera, commandList, fb](const auto& tc, const auto& smc)
			{
				const auto md = MeshFactory::GetMeshData(smc.meshUuid);

				// Fill the constant buffer
				PerObjectConstBuffer constBufferData = {};

				const DirectX::SimpleMath::Matrix world =
					DirectX::SimpleMath::Matrix::CreateScale(tc.scale * md.boundingBox.Extents) *
					DirectX::SimpleMath::Matrix::CreateFromQuaternion(tc.rotation) *
					DirectX::SimpleMath::Matrix::CreateTranslation(tc.position + md.boundingBox.Center);

				constBufferData.gWorldViewProj =
					world * CameraUtils::GetViewProj(origin, *camera);

				commandList->writeBuffer(DefaultAssetLoader::GetEditorGridCB(),
			                             &constBufferData,
			                             sizeof(constBufferData));

				// Set the graphics state: pipeline, framebuffer, viewport, bindings.
				auto graphicsState =
					nvrhi::GraphicsState()
						.setPipeline(
							ShaderFactory::GetGraphicsPipeline("LineList"))
						.setFramebuffer(fb)
						.setViewport(
							nvrhi::ViewportState().addViewportAndScissorRect(
								nvrhi::Viewport(1280, 720)))
						.addBindingSet(ShaderFactory::GetBindingSetV(
							kDebugLineBindingSetUUID))
						.addBindingSet(ShaderFactory::GetBindingSetP(
							kDebugLineBindingSetUUID))
						.addVertexBuffer(
							MeshFactory::GetVertexBufferBinding(kCubeUUID));
				graphicsState.setIndexBuffer(
					MeshFactory::GetIndexBufferBinding(kCubeUUID));
				commandList->setGraphicsState(graphicsState);

				// Draw our geometry
				auto drawArguments = nvrhi::DrawArguments().setVertexCount(
					MeshFactory::GetMeshData(kCubeUUID).indices.size());
				commandList->drawIndexed(drawArguments);
			});
	}
}

void D3E::EditorUtilsRenderSystem::Update(entt::registry& reg, D3E::Game* game,
                                          float dT)
{
	if (isSelectionDirty)
	{
		auto view = reg.view<const ObjectInfoComponent, const TransformComponent, StaticMeshComponent>();

		view.each([game](const auto& info, const auto& tc, auto& smc)
		          {
					  smc.editorHighlighted = game->IsUuidEditorSelected(info.id);
				  });
		isSelectionDirty = false;
	}
}
