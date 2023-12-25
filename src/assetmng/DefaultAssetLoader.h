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

		static nvrhi::BufferHandle gridCb;

	public:
		static void LoadPrimitiveMeshes();
		static void FillPrimitiveMeshBuffers(nvrhi::DeviceHandle& device, nvrhi::CommandListHandle& commandList);
		static void LoadDefaultPSOs(nvrhi::IFramebuffer* fb, nvrhi::IFramebuffer* gBuffFb);
		static void LoadDefaultSamplers(nvrhi::DeviceHandle& device);
		static void LoadEditorDebugAssets(nvrhi::DeviceHandle& device, nvrhi::CommandListHandle& commandList);
		static void LoadDefaultMaterials();

		static nvrhi::IBuffer* GetEditorGridCB();
	};
}
