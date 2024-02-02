#include "LightInitSystem.h"

#include "D3E/CommonCpp.h"
#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Debug.h"
#include "D3E/Game.h"
#include "D3E/components/render/LightComponent.h"
#include "assetmng/MeshFactory.h"
#include "assetmng/TextureFactory.h"
#include "render/CameraUtils.h"
#include "render/CsmConstBuffer.h"
#include "render/GBuffer.h"
#include "render/GameRender.h"
#include "render/LightConstBuffer.h"
#include "render/LightVolumeConstBuffer.h"
#include "render/PointLightConstBuffer.h"
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

	view.each([&, device](const auto& info, auto& lc)
	          {
				  if (lc.initialized)
				  {
					  return;
				  }

				  if (!lc.lightCBuffer)
				  {
					  int constBufferSize =
						  std::max(sizeof(LightConstBuffer),
			                       sizeof(PointLightConstBuffer));

					  auto lightCBufferDesc = nvrhi::BufferDesc()
			                                      .setByteSize(constBufferSize)
			                                      .setIsConstantBuffer(true)
			                                      .setIsVolatile(false)
			                                      .setMaxVersions(16)
			                                      .setKeepInitialState(true);
					  lc.lightCBuffer = device->createBuffer(lightCBufferDesc);
				  }

				  if (!lc.volumeCBuffer)
				  {
					  auto volumeCBufferDesc =
						  nvrhi::BufferDesc()
							  .setByteSize(sizeof(LightVolumeConstBuffer))
							  .setIsConstantBuffer(true)
							  .setIsVolatile(false)
							  .setMaxVersions(16)
							  .setKeepInitialState(true);
					  lc.volumeCBuffer =
						  device->createBuffer(volumeCBufferDesc);
				  }

				  if (lc.lightType == LightType::Directional)
				  {
					  AddDirectionalBSets(info.id, lc);
				  }
				  else if (lc.lightType == LightType::Point)
				  {
					  AddPointBSets(info.id, lc);
				  }

				  lc.initialized = true;
			  });

	LightInitSystem::IsDirty = false;
}

D3E::LightInitSystem::LightInitSystem(D3E::Game* game) : game_(game)
{
}

void D3E::LightInitSystem::AddDirectionalBSets(const D3E::String& uuid, const LightComponent& lc)
{
	ShaderFactory::RemoveBindingSetV(uuid);
	ShaderFactory::RemoveBindingSetP(uuid);

	nvrhi::BindingSetDesc bindingSetDescV = {};
	ShaderFactory::AddBindingSetV(uuid, bindingSetDescV,
	                              "LightPassV");

	nvrhi::BindingSetDesc bindingSetDescP = {};
	bindingSetDescP.addItem(
		nvrhi::BindingSetItem::ConstantBuffer(
			0, lc.lightCBuffer));
	bindingSetDescP.addItem(
		nvrhi::BindingSetItem::ConstantBuffer(
			1, game_->GetRender()->GetCsmConstantBuffer()));
	bindingSetDescP.addItem(
		nvrhi::BindingSetItem::Texture_SRV(
			0, TextureFactory::GetGBuffer()->albedoBuffer));
	bindingSetDescP.addItem(
		nvrhi::BindingSetItem::Texture_SRV(
			1, TextureFactory::GetGBuffer()->positionBuffer));
	bindingSetDescP.addItem(
		nvrhi::BindingSetItem::Texture_SRV(
			2, TextureFactory::GetGBuffer()->normalBuffer));
	bindingSetDescP.addItem(
		nvrhi::BindingSetItem::Texture_SRV(
			3, TextureFactory::GetGBuffer()
				   ->metalRoughnessBuffer));
	bindingSetDescP.addItem(
		nvrhi::BindingSetItem::Texture_SRV(
			4, game_->GetRender()->GetCsmTexture()));
	bindingSetDescP.addItem(nvrhi::BindingSetItem::Sampler(
		0, TextureFactory::GetSampler("BaseGraphics")));
	bindingSetDescP.addItem(nvrhi::BindingSetItem::Sampler(
		1, game_->GetRender()->GetCsmSampler()));
	ShaderFactory::AddBindingSetP(uuid, bindingSetDescP,
	                              "LightPassP");
}

void D3E::LightInitSystem::AddPointBSets(const D3E::String& uuid,
                                         const D3E::LightComponent& lc)
{
	ShaderFactory::RemoveBindingSetV(uuid);
	ShaderFactory::RemoveBindingSetP(uuid);

	nvrhi::BindingSetDesc bindingSetDescV = {};
	bindingSetDescV.addItem(nvrhi::BindingSetItem::ConstantBuffer(0, lc.volumeCBuffer));
	ShaderFactory::AddBindingSetV(uuid, bindingSetDescV,
	                              "PointLightPassV");

	nvrhi::BindingSetDesc bindingSetDescP = {};
	bindingSetDescP.addItem(
		nvrhi::BindingSetItem::ConstantBuffer(
			0, lc.lightCBuffer));
	bindingSetDescP.addItem(
		nvrhi::BindingSetItem::Texture_SRV(
			0, TextureFactory::GetGBuffer()->albedoBuffer));
	bindingSetDescP.addItem(
		nvrhi::BindingSetItem::Texture_SRV(
			1, TextureFactory::GetGBuffer()->positionBuffer));
	bindingSetDescP.addItem(
		nvrhi::BindingSetItem::Texture_SRV(
			2, TextureFactory::GetGBuffer()->normalBuffer));
	bindingSetDescP.addItem(
		nvrhi::BindingSetItem::Texture_SRV(
			3, TextureFactory::GetGBuffer()
				   ->metalRoughnessBuffer));
	bindingSetDescP.addItem(nvrhi::BindingSetItem::Sampler(
		0, TextureFactory::GetSampler("BaseGraphics")));
	ShaderFactory::AddBindingSetP(uuid, bindingSetDescP,
	                              "PointLightPassP");
}
