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

void D3E::RenderUtils::GenerateMips(nvrhi::ITexture* texture, nvrhi::IDevice* device, nvrhi::ICommandList* commandList)
{
	assert(texture);
	assert(texture->getDesc().width == texture->getDesc().height);
	assert((texture->getDesc().width & (texture->getDesc().width - 1)) == 0);

	auto desc = texture->getDesc();

	nvrhi::ComputeState state = {};

	if (desc.arraySize == 1)
	{
		state.setPipeline(
			ShaderFactory::GetComputePipeline("DownsampleLinear"));
	}
	else
	{
		state.setPipeline(
			ShaderFactory::GetComputePipeline("DownsampleArray"));
	}

	nvrhi::BindingSetDesc downsampleBSC = {};
	downsampleBSC.addItem(nvrhi::BindingSetItem::Texture_SRV(0, texture));
	downsampleBSC.addItem(nvrhi::BindingSetItem::Texture_UAV(0, texture));
	auto bindingSet = device->createBindingSet(downsampleBSC, ShaderFactory::GetBindingLayout("DownsampleC"));
	state.addBindingSet(bindingSet);

	commandList->open();
	commandList->setComputeState(state);

	for(UINT level = 1, levelWidth=desc.width / 2, levelHeight=desc.height / 2; level < desc.mipLevels; ++level, levelWidth/=2, levelHeight/=2)
	{
		auto srvSub = nvrhi::TextureSubresourceSet().setMipLevels(level-1, 1);
		auto uavSub = nvrhi::TextureSubresourceSet().setMipLevels(level, 1);
		commandList->setTextureState(texture, srvSub, nvrhi::ResourceStates::ShaderResource);
		commandList->setTextureState(texture, uavSub, nvrhi::ResourceStates::UnorderedAccess);

		commandList->dispatch(std::max(UINT(1), levelWidth/8), std::max(UINT(1), levelHeight/8), desc.arraySize);
	}

	commandList->close();

	device->executeCommandList(commandList);
}
