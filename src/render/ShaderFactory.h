#pragma once

#include "EASTL/vector.h"
#include "EASTL/unordered_map.h"
#include "EASTL/string.h"
#include "nvrhi/nvrhi.h"

namespace D3E
{
	class Game;

	class ShaderFactory
	{
	private:
		static bool isInitialized_;
		static Game* activeGame_;
		static eastl::unordered_map<eastl::string, nvrhi::InputLayoutHandle> iLayouts_;
		static eastl::unordered_map<eastl::string, nvrhi::BindingLayoutHandle> bLayouts_;
		static eastl::unordered_map<eastl::string, nvrhi::BindingSetHandle> bSets_;
		static eastl::unordered_map<eastl::string, nvrhi::ShaderHandle> vShaders_;
		static eastl::unordered_map<eastl::string, nvrhi::ShaderHandle> pShaders_;
		static eastl::unordered_map<eastl::string, nvrhi::ShaderHandle> gShaders_;
		static eastl::unordered_map<eastl::string, nvrhi::ShaderHandle> cShaders_;
		static eastl::unordered_map<eastl::string, nvrhi::GraphicsPipelineHandle> gPipelines_;

		static eastl::string GetBinaryShaderFileName(const eastl::string& fileName, const eastl::string& entryPoint);

	public:
		ShaderFactory() = delete;
		static void Initialize(Game* game);
		static bool IsInitialized() { return isInitialized_; }
		static void DestroyResources();

		static const nvrhi::InputLayoutHandle& AddInputLayout(const eastl::string& name, nvrhi::VertexAttributeDesc* desc, uint32_t count, const nvrhi::ShaderHandle& vs);
		static const nvrhi::BindingLayoutHandle& AddBindingLayout(const eastl::string& name, const nvrhi::BindingLayoutDesc& desc);
		static const nvrhi::BindingSetHandle& AddBindingSet(const eastl::string& name, const nvrhi::BindingSetDesc& desc, const eastl::string& bLayoutName);
		static const nvrhi::ShaderHandle& AddVertexShader(const eastl::string& name, const eastl::string& fileName, const eastl::string& entryPoint);
		static const nvrhi::ShaderHandle& AddPixelShader(const eastl::string& name, const eastl::string& fileName, const eastl::string& entryPoint);
		static const nvrhi::ShaderHandle& AddGeometryShader(const eastl::string& name, const eastl::string& fileName, const eastl::string& entryPoint);
		static const nvrhi::ShaderHandle& AddComputeShader(const eastl::string& name, const eastl::string& fileName, const eastl::string& entryPoint);
		static const nvrhi::GraphicsPipelineHandle& AddGraphicsPipeline(const eastl::string& name, const nvrhi::GraphicsPipelineDesc& desc, const nvrhi::FramebufferHandle& fb);

		static const nvrhi::InputLayoutHandle& GetInputLayout(const eastl::string& name);
		static const nvrhi::BindingLayoutHandle& GetBindingLayout(const eastl::string& name);
		static const nvrhi::BindingSetHandle& GetBindingSet(const eastl::string& name);
		static const nvrhi::ShaderHandle& GetVertexShader(const eastl::string& name);
		static const nvrhi::ShaderHandle& GetPixelShader(const eastl::string& name);
		static const nvrhi::ShaderHandle& GetGeometryShader(const eastl::string& name);
		static const nvrhi::ShaderHandle& GetComputeShader(const eastl::string& name);
		static const nvrhi::GraphicsPipelineHandle& GetGraphicsPipeline(const eastl::string& name);
	};
}
