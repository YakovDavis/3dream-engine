#include "LightInitSystem.h"

#include "D3E/CommonCpp.h"
#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Debug.h"
#include "D3E/components/render/LightComponent.h"
#include "assetmng/MeshFactory.h"
#include "assetmng/TextureFactory.h"
#include "render/CameraUtils.h"
#include "render/CsmConstBuffer.h"
#include "render/GBuffer.h"
#include "render/LightConstBuffer.h"
#include "render/ShaderFactory.h"

bool D3E::LightInitSystem::IsDirty = false;

void D3E::LightInitSystem::PreDraw(entt::registry& reg, nvrhi::ICommandList* commandList,
                                        nvrhi::IDevice* device)
{
	if (!LightInitSystem::IsDirty)
	{
		return;
	}

	auto view = reg.view<const ObjectInfoComponent, LightComponent>();

	view.each([device, commandList](const auto& info, auto& lc)
	          {
				  if (lc.initialized)
				  {
					  return;
				  }

				  auto lightCBufferDesc = nvrhi::BufferDesc()
		                                        .setByteSize(sizeof(LightConstBuffer))
		                                        .setIsConstantBuffer(true)
		                                        .setIsVolatile(false)
		                                        .setMaxVersions(16)
		                                        .setKeepInitialState(true);

				  lc.lightCBuffer = device->createBuffer(lightCBufferDesc);

				  auto csmCBufferDesc = nvrhi::BufferDesc()
		                                        .setByteSize(sizeof(CsmConstBuffer))
		                                        .setIsConstantBuffer(true)
		                                        .setIsVolatile(false)
		                                        .setMaxVersions(16)
		                                        .setKeepInitialState(true);

				  lc.csmCBuffer = device->createBuffer(csmCBufferDesc);

				  nvrhi::BindingSetDesc bindingSetDescV = {};
				  ShaderFactory::AddBindingSetV(info.id, bindingSetDescV, "LightPassV");

				  nvrhi::BindingSetDesc bindingSetDescP = {};
				  bindingSetDescP.addItem(nvrhi::BindingSetItem::ConstantBuffer(0, lc.lightCBuffer));
				  bindingSetDescP.addItem(nvrhi::BindingSetItem::ConstantBuffer(1, lc.csmCBuffer));
				  bindingSetDescP.addItem(nvrhi::BindingSetItem::Texture_SRV(0, TextureFactory::GetGBuffer()->albedoBuffer));
				  bindingSetDescP.addItem(nvrhi::BindingSetItem::Texture_SRV(1, TextureFactory::GetGBuffer()->positionBuffer));
				  bindingSetDescP.addItem(nvrhi::BindingSetItem::Texture_SRV(2, TextureFactory::GetGBuffer()->normalBuffer));
				  bindingSetDescP.addItem(nvrhi::BindingSetItem::Texture_SRV(3, TextureFactory::GetGBuffer()->metalRoughnessBuffer));
				  bindingSetDescP.addItem(nvrhi::BindingSetItem::Texture_SRV(4, TextureFactory::GetTextureHandle(kEnvTextureUUID)));
				  bindingSetDescP.addItem(nvrhi::BindingSetItem::Texture_SRV(5, TextureFactory::GetTextureHandle(kIrMapTextureUUID)));
				  bindingSetDescP.addItem(nvrhi::BindingSetItem::Texture_SRV(6, TextureFactory::GetTextureHandle(kSpBrdfLutTextureUUID)));
				  bindingSetDescP.addItem(nvrhi::BindingSetItem::Texture_SRV(7, TextureFactory::GetTextureHandle("34b9a6f1-240f-4d40-b76d-ad38ce9e65ea")));
				  bindingSetDescP.addItem(nvrhi::BindingSetItem::Sampler(0, TextureFactory::GetSampler("Base")));
				  bindingSetDescP.addItem(nvrhi::BindingSetItem::Sampler(1, TextureFactory::GetSampler("SpBrdf")));
				  bindingSetDescP.addItem(nvrhi::BindingSetItem::Sampler(2, TextureFactory::GetSampler("Base")));
				  ShaderFactory::AddBindingSetP(info.id, bindingSetDescP, "LightPassP");

				  lc.initialized = true;
			  });

	LightInitSystem::IsDirty = false;
}
