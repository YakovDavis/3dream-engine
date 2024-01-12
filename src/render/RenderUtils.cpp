#include "RenderUtils.h"

#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Components/render/LightComponent.h"
#include "D3E/Debug.h"
#include "D3E/components/render/StaticMeshComponent.h"
#include "ShaderFactory.h"

void D3E::RenderUtils::InvalidateWorldBuffers(entt::registry& reg)
{
	{
		auto view = reg.view<const ObjectInfoComponent, StaticMeshComponent>();

		view.each(
			[](const auto& info, auto& smc)
			{
				if (!smc.initialized)
				{
					return;
				}

				smc.constantBuffer = nullptr;

				ShaderFactory::RemoveBindingSetV(info.id);
				ShaderFactory::RemoveBindingSetP(info.id);

				smc.initialized = false;
			});
	}

	{
		auto view = reg.view<const ObjectInfoComponent, LightComponent>();

		view.each([](const auto& info, auto& lc)
			{
				if (!lc.initialized)
				{
					return;
				}

				lc.lightCBuffer = nullptr;

				lc.csmCBuffer = nullptr;

				ShaderFactory::RemoveBindingSetV(info.id);
				ShaderFactory::RemoveBindingSetP(info.id);

				lc.initialized = false;
			});
	}
}
