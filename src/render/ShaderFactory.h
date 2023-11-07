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
		static eastl::unordered_map<eastl::string, nvrhi::ShaderHandle> vShaders_;
		static eastl::unordered_map<eastl::string, nvrhi::ShaderHandle> pShaders_;
		static eastl::unordered_map<eastl::string, nvrhi::ShaderHandle> gShaders_;
		static eastl::unordered_map<eastl::string, nvrhi::ShaderHandle> cShaders_;

		static eastl::string GetBinaryShaderFileName(const eastl::string& fileName, const eastl::string& entryPoint);

	public:
		ShaderFactory() = delete;
		static void Initialize(Game* game);
		static bool IsInitialized() { return isInitialized_; }
		static void DestroyResources();

		static nvrhi::InputLayoutHandle AddInputLayout(const eastl::string& name, nvrhi::VertexAttributeDesc* desc, uint32_t count, const nvrhi::ShaderHandle& vs);
		static nvrhi::InputLayoutHandle AddBindingLayout(const eastl::string& name, nvrhi::BindingLayoutDesc desc);
		static nvrhi::ShaderHandle AddVertexShader(const eastl::string& name, const eastl::string& fileName, const eastl::string& entryPoint);
		static nvrhi::ShaderHandle AddPixelShader(const eastl::string& name, const eastl::string& fileName, const eastl::string& entryPoint);
		static nvrhi::ShaderHandle AddGeometryShader(const eastl::string& name, const eastl::string& fileName, const eastl::string& entryPoint);
		static nvrhi::ShaderHandle AddComputeShader(const eastl::string& name, const eastl::string& fileName, const eastl::string& entryPoint);

		static nvrhi::InputLayoutHandle GetInputLayout(const eastl::string& name);
		static nvrhi::InputLayoutHandle GetBindingLayout(const eastl::string& name);
		static nvrhi::ShaderHandle GetVertexShader(const eastl::string& name);
		static nvrhi::ShaderHandle GetPixelShader(const eastl::string& name);
		static nvrhi::ShaderHandle GetGeometryShader(const eastl::string& name);
		static nvrhi::ShaderHandle GetComputeShader(const eastl::string& name);
	};
}