#include "PbrUtils.h"

#include "RenderUtils.h"
#include "ShaderFactory.h"
#include "assetmng/TextureFactory.h"

void D3E::PbrUtils::Setup(nvrhi::IDevice* device, nvrhi::ICommandList* commandList)
{
	nvrhi::TextureHandle m_envTexture = TextureFactory::GetNewTextureHandle(kEnvTextureUUID);
	nvrhi::TextureHandle m_irmapTexture = TextureFactory::GetNewTextureHandle(kIrMapTextureUUID);
	nvrhi::TextureHandle m_spBRDF_LUT = TextureFactory::GetNewTextureHandle(kSpBrdfLutTextureUUID);

	{
		// Unfiltered environment cube map (temporary).
		nvrhi::TextureDesc envTextureUnfilteredDesc = {};
		envTextureUnfilteredDesc.setDimension(nvrhi::TextureDimension::TextureCube);
		envTextureUnfilteredDesc.setArraySize(6);
		envTextureUnfilteredDesc.setWidth(1024);
		envTextureUnfilteredDesc.setHeight(1024);
		envTextureUnfilteredDesc.setMipLevels(10);
		envTextureUnfilteredDesc.setFormat(nvrhi::Format::RGBA16_UNORM);
		envTextureUnfilteredDesc.setIsUAV(true);
		envTextureUnfilteredDesc.setInitialState(nvrhi::ResourceStates::UnorderedAccess);
		envTextureUnfilteredDesc.setKeepInitialState(true);

		nvrhi::TextureHandle envTextureUnfiltered = device->createTexture(envTextureUnfilteredDesc);

		// Load & convert equirectangular environment map to a cubemap texture.
		{
			nvrhi::ITexture* envTextureEquirect = TextureFactory::GetTextureHandle("e408d7c8-f1ad-42fb-a0bd-35669f30d7a1");

			nvrhi::BindingSetDesc equirect2CubeBSC = {};
			equirect2CubeBSC.addItem(nvrhi::BindingSetItem::Texture_SRV(0, envTextureEquirect));
			equirect2CubeBSC.addItem(nvrhi::BindingSetItem::Texture_UAV(0, envTextureUnfiltered));
			equirect2CubeBSC.addItem(nvrhi::BindingSetItem::Sampler(0, TextureFactory::GetSampler("Base")));
			ShaderFactory::AddBindingSetC("Equirect2Cube", equirect2CubeBSC, "Equirect2CubeC");

			nvrhi::ComputeState computeState = {};
			computeState.pipeline = ShaderFactory::GetComputePipeline("Equirect2Cube");
			computeState.bindings = { ShaderFactory::GetBindingSetC("Equirect2Cube") };

			commandList->open();
			commandList->setComputeState(computeState);
			commandList->dispatch(envTextureUnfilteredDesc.width/32, envTextureUnfilteredDesc.height/32, 6);
			commandList->close();
			device->executeCommandList(commandList);
		}

		RenderUtils::GenerateMips(envTextureUnfiltered, device, commandList);

		// Compute pre-filtered specular environment map.
		{
			struct SpecularMapFilterSettingsCB
			{
				float roughness;
				float padding[3];
			};
			nvrhi::ComputeState computeState = {};
			computeState.pipeline = ShaderFactory::GetComputePipeline("SpMap");

			nvrhi::BufferDesc spmapCbDesc = {};
			spmapCbDesc.setByteSize(sizeof(SpecularMapFilterSettingsCB));
			spmapCbDesc.setIsConstantBuffer(true);
			spmapCbDesc.setIsVolatile(false);
			spmapCbDesc.setMaxVersions(16);
			spmapCbDesc.setKeepInitialState(true);
			nvrhi::BufferHandle spmapCB = device->createBuffer(spmapCbDesc);

			nvrhi::TextureDesc envTextureDesc = {};
			envTextureDesc.setDimension(nvrhi::TextureDimension::TextureCube);
			envTextureDesc.setArraySize(6);
			envTextureDesc.setWidth(1024);
			envTextureDesc.setHeight(1024);
			envTextureDesc.setMipLevels(10);
			envTextureDesc.setFormat(nvrhi::Format::RGBA16_FLOAT);
			envTextureDesc.setIsUAV(true);
			envTextureDesc.setInitialState(nvrhi::ResourceStates::Common);
			//envTextureDesc.setKeepInitialState(true);
			m_envTexture = device->createTexture(envTextureUnfilteredDesc);

			commandList->open();

			// Copy 0th mipmap level into destination environment map.
			for(int arraySlice=0; arraySlice<6; ++arraySlice)
			{
				auto currentSlice = nvrhi::TextureSlice().setMipLevel(0);
				commandList->copyTexture(m_envTexture, currentSlice, envTextureUnfiltered, currentSlice);
			}

			// Pre-filter rest of the mip chain.
			const float deltaRoughness = 1.0f / std::max(float(m_envTexture->getDesc().mipLevels - 1), 1.0f);
			for(UINT level = 1, size = 512; level<m_envTexture->getDesc().mipLevels; ++level, size /= 2)
			{
				const UINT numGroups = std::max<UINT>(1, size/32);
				auto currentSubresource = nvrhi::TextureSubresourceSet().setMipLevels(level, 1);
				commandList->setTextureState(envTextureUnfiltered, currentSubresource, nvrhi::ResourceStates::UnorderedAccess);

				const SpecularMapFilterSettingsCB spmapConstants = { level * deltaRoughness };

				commandList->writeBuffer(spmapCB, &spmapConstants, sizeof(SpecularMapFilterSettingsCB));

				nvrhi::BindingSetDesc bindingSetDesc = {};
				bindingSetDesc.addItem(nvrhi::BindingSetItem::ConstantBuffer(0, spmapCB));
				bindingSetDesc.addItem(nvrhi::BindingSetItem::Texture_SRV(0, envTextureUnfiltered));
				bindingSetDesc.addItem(nvrhi::BindingSetItem::Texture_UAV(0, m_envTexture));
				bindingSetDesc.addItem(nvrhi::BindingSetItem::Sampler(0, TextureFactory::GetSampler("Base")));
				nvrhi::BindingSetHandle bindingSetHandle = device->createBindingSet(bindingSetDesc, ShaderFactory::GetBindingLayout("SpMapC"));

				computeState.bindings = { bindingSetHandle };

				commandList->setComputeState(computeState);

				commandList->dispatch(numGroups, numGroups, 6);
			}

			commandList->close();
			device->executeCommandList(commandList);
		}
	}

	// Compute diffuse irradiance cubemap.
	{
		nvrhi::ComputeState computeState = {};
		computeState.pipeline = ShaderFactory::GetComputePipeline("IrMap");

		nvrhi::TextureDesc irmapTexturDesc = {};
		irmapTexturDesc.setDimension(nvrhi::TextureDimension::TextureCube);
		irmapTexturDesc.setArraySize(6);
		irmapTexturDesc.setWidth(32);
		irmapTexturDesc.setHeight(32);
		irmapTexturDesc.setMipLevels(5);
		irmapTexturDesc.setFormat(nvrhi::Format::RGBA16_FLOAT);
		irmapTexturDesc.setIsUAV(true);
		irmapTexturDesc.setInitialState(nvrhi::ResourceStates::UnorderedAccess);
		m_irmapTexture = device->createTexture(irmapTexturDesc);

		nvrhi::BindingSetDesc bindingSetDesc = {};
		bindingSetDesc.addItem(nvrhi::BindingSetItem::Texture_SRV(0, m_envTexture));
		bindingSetDesc.addItem(nvrhi::BindingSetItem::Texture_UAV(0, m_irmapTexture));
		bindingSetDesc.addItem(nvrhi::BindingSetItem::Sampler(0, TextureFactory::GetSampler("Base")));
		nvrhi::BindingSetHandle bindingSetHandle = device->createBindingSet(bindingSetDesc, ShaderFactory::GetBindingLayout("IrMapC"));
		computeState.bindings = { bindingSetHandle };

		commandList->open();
		commandList->setTextureState(m_envTexture, nvrhi::AllSubresources, nvrhi::ResourceStates::ShaderResource);
		commandList->beginTrackingTextureState(m_irmapTexture, nvrhi::AllSubresources, nvrhi::ResourceStates::UnorderedAccess);
		commandList->setComputeState(computeState);
		commandList->dispatch(m_irmapTexture->getDesc().width / 32, m_irmapTexture->getDesc().height / 32, 6);
		commandList->close();
		device->executeCommandList(commandList);
	}

	// Compute Cook-Torrance BRDF 2D LUT for split-sum approximation.
	{
		nvrhi::ComputeState computeState = {};
		computeState.pipeline = ShaderFactory::GetComputePipeline("SpBrdf");

		nvrhi::TextureDesc spBrdfTexturDesc = {};
		spBrdfTexturDesc.setDimension(nvrhi::TextureDimension::Texture2D);
		spBrdfTexturDesc.setArraySize(1);
		spBrdfTexturDesc.setWidth(256);
		spBrdfTexturDesc.setHeight(256);
		spBrdfTexturDesc.setMipLevels(8);
		spBrdfTexturDesc.setFormat(nvrhi::Format::RGBA16_FLOAT);
		spBrdfTexturDesc.setIsUAV(true);
		spBrdfTexturDesc.setInitialState(nvrhi::ResourceStates::UnorderedAccess);
		m_spBRDF_LUT = device->createTexture(spBrdfTexturDesc);

		nvrhi::SamplerDesc samplerDesc = {};
		samplerDesc.magFilter = true;
		samplerDesc.minFilter = true;
		samplerDesc.mipFilter = true;
		samplerDesc.addressU = nvrhi::SamplerAddressMode::Clamp;
		samplerDesc.addressV = nvrhi::SamplerAddressMode::Clamp;
		samplerDesc.addressW = nvrhi::SamplerAddressMode::Clamp;
		TextureFactory::AddSampler("SpBrdf", device, samplerDesc);

		nvrhi::BindingSetDesc bindingSetDesc = {};
		bindingSetDesc.addItem(nvrhi::BindingSetItem::Texture_UAV(0, m_spBRDF_LUT));
		nvrhi::BindingSetHandle bindingSetHandle = device->createBindingSet(bindingSetDesc, ShaderFactory::GetBindingLayout("SpBrdfC"));
		computeState.bindings = { bindingSetHandle };

		commandList->open();
		commandList->beginTrackingTextureState(m_spBRDF_LUT, nvrhi::AllSubresources, nvrhi::ResourceStates::UnorderedAccess);
		commandList->setComputeState(computeState);
		commandList->dispatch(m_spBRDF_LUT->getDesc().width / 32, m_spBRDF_LUT->getDesc().height / 32, 1);
		commandList->close();
		device->executeCommandList(commandList);
	}
}
