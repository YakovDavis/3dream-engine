#pragma once

#include "D3E/CommonHeader.h"
#include "EASTL/unordered_map.h"
#include "EASTL/vector.h"
#include "nvrhi/nvrhi.h"

namespace D3E
{
	class Game;

	class ShaderFactory
	{
	private:
		static bool isInitialized_;
		static Game* activeGame_;
		static eastl::unordered_map<String, nvrhi::InputLayoutHandle> iLayouts_;
		static eastl::unordered_map<String, nvrhi::BindingLayoutHandle> bLayouts_;
		static eastl::unordered_map<String, nvrhi::BindingSetHandle> bSets_;
		static eastl::unordered_map<String, nvrhi::ShaderHandle> vShaders_;
		static eastl::unordered_map<String, nvrhi::ShaderHandle> pShaders_;
		static eastl::unordered_map<String, nvrhi::ShaderHandle> gShaders_;
		static eastl::unordered_map<String, nvrhi::ShaderHandle> cShaders_;
		static eastl::unordered_map<String, nvrhi::GraphicsPipelineHandle> gPipelines_;

		static String GetBinaryShaderFileName(const String& fileName, const String& entryPoint);

	public:
		ShaderFactory() = delete;
		static void Initialize(Game* game);
		static bool IsInitialized() { return isInitialized_; }
		static void DestroyResources();

		static const nvrhi::InputLayoutHandle& AddInputLayout(const String& name, nvrhi::VertexAttributeDesc* desc, uint32_t count, const nvrhi::ShaderHandle& vs);
		static const nvrhi::BindingLayoutHandle& AddBindingLayout(const String& name, const nvrhi::BindingLayoutDesc& desc);
		static const nvrhi::BindingSetHandle& AddBindingSet(const String& name, const nvrhi::BindingSetDesc& desc, const String& bLayoutName);
		static const nvrhi::ShaderHandle& AddVertexShader(const String& name, const String& fileName, const String& entryPoint);
		static const nvrhi::ShaderHandle& AddPixelShader(const String& name, const String& fileName, const String& entryPoint);
		static const nvrhi::ShaderHandle& AddGeometryShader(const String& name, const String& fileName, const String& entryPoint);
		static const nvrhi::ShaderHandle& AddComputeShader(const String& name, const String& fileName, const String& entryPoint);
		static const nvrhi::GraphicsPipelineHandle& AddGraphicsPipeline(const String& name, const nvrhi::GraphicsPipelineDesc& desc, const nvrhi::FramebufferHandle& fb);

		static const nvrhi::InputLayoutHandle& GetInputLayout(const String& name);
		static const nvrhi::BindingLayoutHandle& GetBindingLayout(const String& name);
		static const nvrhi::BindingSetHandle& GetBindingSet(const String& name);
		static const nvrhi::ShaderHandle& GetVertexShader(const String& name);
		static const nvrhi::ShaderHandle& GetPixelShader(const String& name);
		static const nvrhi::ShaderHandle& GetGeometryShader(const String& name);
		static const nvrhi::ShaderHandle& GetComputeShader(const String& name);
		static const nvrhi::GraphicsPipelineHandle& GetGraphicsPipeline(const String& name);
	};
}
