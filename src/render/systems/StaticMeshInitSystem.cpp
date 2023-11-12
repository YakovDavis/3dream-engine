#include "StaticMeshInitSystem.h"

#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Debug.h"
#include "D3E/components/render/StaticMeshComponent.h"
#include "assetmng/MeshFactory.h"
#include "assetmng/TextureFactory.h"
#include "render/CameraUtils.h"
#include "render/PerObjectConstBuffer.h"
#include "render/ShaderFactory.h"

void D3E::StaticMeshInitSystem::Run(entt::registry& reg, nvrhi::IDevice* device,
                                    nvrhi::CommandListHandle& commandList)
{
	auto view = reg.view<StaticMeshComponent>();

	view.each([device, commandList](auto &smc)
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
					ShaderFactory::AddBindingSet("SimpleForwardV", bindingSetDescV, "SimpleForwardV");

					nvrhi::BindingSetDesc bindingSetDescP = {};
					bindingSetDescP.addItem(nvrhi::BindingSetItem::Texture_SRV(0, TextureFactory::GetTextureHandle("wood")));
					bindingSetDescP.addItem(nvrhi::BindingSetItem::Sampler(0, TextureFactory::GetSampler("Base")));
					ShaderFactory::AddBindingSet("SimpleForwardP", bindingSetDescP, "SimpleForwardP");

					smc.initialized = true;
			  });
}