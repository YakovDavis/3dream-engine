#pragma once

#include "D3E/CommonHeader.h"
#include "EASTL/vector.h"
#include "nvrhi/nvrhi.h"

namespace D3E
{
	class DefaultAssetLoader
	{
		static eastl::vector<String> loadedVertexShaders;

		static void LoadDefaultInputLayouts();

	public:
		static void LoadPrimitiveMeshes();
		static void FillPrimitiveMeshBuffers(nvrhi::DeviceHandle& device, nvrhi::CommandListHandle& commandList);
		static void LoadDefaultPSOs(nvrhi::IFramebuffer* fb, nvrhi::IFramebuffer* gBuffFb);
		static void LoadDefaultSamplers(nvrhi::DeviceHandle& device);
	};
}
