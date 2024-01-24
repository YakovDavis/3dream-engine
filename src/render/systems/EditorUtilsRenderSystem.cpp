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
