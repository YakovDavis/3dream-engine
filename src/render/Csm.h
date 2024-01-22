#pragma once

#include "D3E/CommonHeader.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Components/render/LightComponent.h"
#include "EASTL/vector.h"
#include "PerObjectConstBuffer.h"
#include "SimpleMath.h"
#include "nvrhi/nvrhi.h"

#define CASCADE_COUNT 4

namespace D3E
{
	class Game;

	class Csm
	{
	private:
		static Csm* instance_;

	public:
		static Csm* Init(Game *game);
		static Csm* Get() { return instance_; }

		void GenerateCascadeMaps();
		nvrhi::IBuffer* GetCsmConstantBuffer() { return gsCB_; }
		nvrhi::ITexture* GetCsmTexture() { return csmTexture_; }
		nvrhi::ISampler* GetCsmSampler() { return csmSampler_; }

		static void DestroyResources();

	private:
#pragma pack(16)
		struct CascadeConstBuffer
		{
			DirectX::SimpleMath::Matrix gViewProj[CASCADE_COUNT + 1];
			DirectX::SimpleMath::Vector4 gDistances;
		};

		eastl::vector<float> shadowCascadeLevels_{};

		DirectX::SimpleMath::Matrix GetLightSpaceMatrix(const LightComponent& lightComponent,
		                                                const DirectX::SimpleMath::Vector3& parentLoc,
		                                                const CameraComponent& cameraComponent,
		                                                float nearPlane,
		                                                float farPlane);
		DirectX::SimpleMath::Vector4 GetShadowCascadeDistances() const;
		eastl::vector<DirectX::SimpleMath::Matrix> GetLightSpaceMatrices(const LightComponent& lightComponent,
		                                                               const DirectX::SimpleMath::Vector3& parentLoc,
		                                                               const CameraComponent& cameraComponent);

		Game *game_;

		nvrhi::DeviceHandle device_;
		nvrhi::CommandListHandle commandList_;

		PerObjectConstBuffer vsCBData_;
		CascadeConstBuffer gsCBData_;

		nvrhi::BufferHandle vsCB_;
		nvrhi::BufferHandle gsCB_;

		nvrhi::BindingSetHandle vsBS_;
		nvrhi::BindingSetHandle gsBS_;

		nvrhi::FramebufferHandle fb_;
		nvrhi::TextureHandle csmTexture_;
		nvrhi::GraphicsPipelineHandle pso_;
		nvrhi::GraphicsState graphicsState_;

		nvrhi::SamplerHandle csmSampler_;

	private:
		explicit Csm(Game *game);
	};
}
