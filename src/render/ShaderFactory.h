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
		static eastl::unordered_map<eastl::string, nvrhi::ShaderHandle> vShaders_;
		static eastl::unordered_map<eastl::string, nvrhi::ShaderHandle> pShaders_;
		static eastl::unordered_map<eastl::string, nvrhi::ShaderHandle> gShaders_;

		static eastl::string GetBinaryShaderFileName(const eastl::string& fileName, const eastl::string& entryPoint);

	public:
		ShaderFactory() = delete;
		static void Initialize(Game* game);
		static bool IsInitialized() { return isInitialized_; }
		static void DestroyResources();

		static nvrhi::ShaderHandle AddVertexShader(const eastl::string& name, const eastl::string& fileName, const eastl::string& entryPoint);
		static nvrhi::ShaderHandle AddPixelShader(const eastl::string& name, const eastl::string& fileName, const eastl::string& entryPoint);
		static nvrhi::ShaderHandle AddGeometryShader(const eastl::string& name, const eastl::string& fileName, const eastl::string& entryPoint);

		static nvrhi::ShaderHandle GetVertexShader(const eastl::string& name);
		static nvrhi::ShaderHandle GetPixelShader(const eastl::string& name);
		static nvrhi::ShaderHandle GetGeometryShader(const eastl::string& name);
	};
}