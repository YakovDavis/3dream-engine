#pragma once

#include "nvrhi/nvrhi.h"

namespace D3E
{
	class Display;

	struct GBuffer
	{
		nvrhi::TextureHandle albedoBuffer;
		nvrhi::TextureHandle positionBuffer;
		nvrhi::TextureHandle normalBuffer;
		nvrhi::TextureHandle metalRoughnessBuffer;

		void Initialize(nvrhi::IDevice* device, nvrhi::ICommandList* commandList, Display* display);
	};
}
