#include "StaticMeshInitSystem.h"

#include "D3E/CommonCpp.h"
#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Debug.h"
#include "D3E/components/render/StaticMeshComponent.h"
#include "assetmng/TextureFactory.h"
#include "render/PerObjectConstBuffer.h"
#include "render/ShaderFactory.h"

bool D3E::StaticMeshInitSystem::IsDirty = false;

void D3E::StaticMeshInitSystem::PreDraw(entt::registry& reg, nvrhi::ICommandList* commandList,
                                        nvrhi::IDevice* device)
{
	if (!StaticMeshInitSystem::IsDirty)
	{
		return;
	}

	auto view = reg.view<const ObjectInfoComponent, StaticMeshComponent>();

	view.each([device, commandList](const auto& info, auto& smc)
	          {
					if (smc.initialized)
					{
						return;
					}

					auto constantBufferDesc = nvrhi::BufferDesc()
		                                          .setByteSize(sizeof(PerObjectConstBuffer))
		                                          .setIsConstantBuffer(true)
		                                          .setIsVolatile(false)
		                                          .setMaxVersions(16)
		                                          .setKeepInitialState(true);

					smc.constantBuffer = device->createBuffer(constantBufferDesc);

					nvrhi::BindingSetDesc bindingSetDescV = {};
					bindingSetDescV.addItem(nvrhi::BindingSetItem::ConstantBuffer(0, smc.constantBuffer));
					ShaderFactory::AddBindingSetV(info.id, bindingSetDescV, "GBufferV");

					nvrhi::BindingSetDesc bindingSetDescP = {};
					bindingSetDescP.addItem(nvrhi::BindingSetItem::Texture_SRV(0, TextureFactory::GetTextureHandle("24c71f11-0d38-4594-ae18-c8eedca9b896")));
					bindingSetDescP.addItem(nvrhi::BindingSetItem::Texture_SRV(1, TextureFactory::GetTextureHandle("24c71f11-0d38-4594-ae18-c8eedca9b896")));
					bindingSetDescP.addItem(nvrhi::BindingSetItem::Texture_SRV(2, TextureFactory::GetTextureHandle("24c71f11-0d38-4594-ae18-c8eedca9b896")));
					bindingSetDescP.addItem(nvrhi::BindingSetItem::Texture_SRV(3, TextureFactory::GetTextureHandle("24c71f11-0d38-4594-ae18-c8eedca9b896")));
					bindingSetDescP.addItem(nvrhi::BindingSetItem::Texture_SRV(4, TextureFactory::GetTextureHandle("24c71f11-0d38-4594-ae18-c8eedca9b896")));
					bindingSetDescP.addItem(nvrhi::BindingSetItem::Texture_SRV(5, TextureFactory::GetTextureHandle("24c71f11-0d38-4594-ae18-c8eedca9b896")));
					bindingSetDescP.addItem(nvrhi::BindingSetItem::Sampler(0, TextureFactory::GetSampler("Base")));
					bindingSetDescP.addItem(nvrhi::BindingSetItem::Sampler(1, TextureFactory::GetSampler("Base")));
					ShaderFactory::AddBindingSetP(info.id, bindingSetDescP, "GBufferP");

					smc.initialized = true;
			  });

	StaticMeshInitSystem::IsDirty = false;
}
