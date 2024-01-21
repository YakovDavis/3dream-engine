#include "LightRenderSystem.h"

#include "D3E/CommonCpp.h"
#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Components/render/LightComponent.h"
#include "D3E/Debug.h"
#include "D3E/components/TransformComponent.h"
#include "D3E/engine/ConsoleManager.h"
#include "SimpleMath.h"
#include "core/EngineState.h"
#include "render/LightConstBuffer.h"
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

	auto view = reg.view<const ObjectInfoComponent, const TransformComponent, const LightComponent>();

	view.each([commandList, fb, origin](const auto& info, const auto& tc, const auto& lc)
	          {
				  // Fill the constant buffer
				  LightConstBuffer constBufferData = {};

				  constBufferData.gEyePosition = Vector4(origin.x, origin.y, origin.z, 1.0f);
				  constBufferData.gLightDir = Vector4(lc.direction.x, lc.direction.y, lc.direction.z, 0.0f);
				  constBufferData.gLightDir.Normalize();
				  constBufferData.gLightColor = Vector4(lc.color.x, lc.color.y, lc.color.z, 1.0f);

				  commandList->writeBuffer(lc.lightCBuffer, &constBufferData, sizeof(constBufferData));

				  // Set the graphics state: pipeline, framebuffer, viewport, bindings.
				  nvrhi::GraphicsState graphicsState = {};
		          graphicsState.setPipeline(ShaderFactory::GetGraphicsPipeline("LightPass"));
				  graphicsState.setFramebuffer(fb);
				  graphicsState.setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(EngineState::GetViewportWidth(), EngineState::GetViewportHeight())));
				  graphicsState.addBindingSet(ShaderFactory::GetBindingSetV(info.id));
				  graphicsState.addBindingSet(ShaderFactory::GetBindingSetP(info.id));
				  commandList->setGraphicsState(graphicsState);

				  // Draw our geometry
				  auto drawArguments = nvrhi::DrawArguments()
		                                   .setVertexCount(4);
				  commandList->draw(drawArguments);
			  });
}
