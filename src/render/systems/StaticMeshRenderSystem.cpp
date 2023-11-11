#include "StaticMeshRenderSystem.h"

#include "D3E/components/render/StaticMeshComponent.h"

void D3E::StaticMeshRenderSystem::Render(entt::registry& reg)
{
	auto view = reg.view<StaticMeshComponent>();

	view.each([](const auto entity, auto &smc)
	          {

			  });
}
