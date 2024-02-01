#pragma once

#include "D3E/systems/GameSystem.h"
#include "nvrhi/nvrhi.h"

namespace D3E
{
	class LightRenderSystem : public GameSystem
	{
	public:
		void Draw(entt::registry& reg, nvrhi::IFramebuffer* fb, nvrhi::ICommandList* commandList, nvrhi::IDevice* device) override;

	private:
		nvrhi::BufferHandle iblCBuffer_;
		nvrhi::BindingSetHandle iblBSV_;
		nvrhi::BindingSetHandle iblBSP_;
	};
}
