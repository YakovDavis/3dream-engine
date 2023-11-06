#include "ShaderFactory.h"

#include <fstream>
#include "D3E/Game.h"
#include "render/GameRender.h"
#include <filesystem>
#include "D3E/Debug.h"

bool D3E::ShaderFactory::isInitialized_ = false;
D3E::Game* D3E::ShaderFactory::activeGame_;
eastl::unordered_map<eastl::string, nvrhi::ShaderHandle> D3E::ShaderFactory::vShaders_ {};
eastl::unordered_map<eastl::string, nvrhi::ShaderHandle> D3E::ShaderFactory::pShaders_ {};
eastl::unordered_map<eastl::string, nvrhi::ShaderHandle> D3E::ShaderFactory::gShaders_ {};
eastl::unordered_map<eastl::string, nvrhi::ShaderHandle> D3E::ShaderFactory::cShaders_ {};

void D3E::ShaderFactory::Initialize(Game* game)
{
	if (isInitialized_)
		return;

	activeGame_ = game;
	isInitialized_ = true;
}

void D3E::ShaderFactory::DestroyResources()
{
	if (!isInitialized_)
		return;

	vShaders_.clear();
	gShaders_.clear();
	pShaders_.clear();

	isInitialized_ = false;
}

nvrhi::ShaderHandle D3E::ShaderFactory::AddVertexShader(const eastl::string& name,
                                    					const eastl::string& fileName,
                                                        const eastl::string& entryPoint)
{
	auto adjustedName = GetBinaryShaderFileName(fileName, entryPoint);
	std::ifstream file(adjustedName.c_str(), std::ios::binary | std::ios::ate);
	if (!file.good())
	{
		Debug::LogError(eastl::string("Can't open binary shader file ") + adjustedName + eastl::string(". Try recompiling the shaders."));
	}
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<char> buffer(size);
	file.read(buffer.data(), size);
	file.close();
	vShaders_.insert({name, activeGame_->GetRender()->GetDevice()->createShader(
								nvrhi::ShaderDesc(nvrhi::ShaderType::Vertex),
								&(buffer[0]), buffer.size() * sizeof(char))});
	return vShaders_[name];
}

nvrhi::ShaderHandle D3E::ShaderFactory::AddPixelShader(const eastl::string& name,
                                                       const eastl::string& fileName,
                                                       const eastl::string& entryPoint)
{
	auto adjustedName = GetBinaryShaderFileName(fileName, entryPoint);
	std::ifstream file(adjustedName.c_str(), std::ios::binary | std::ios::ate);
	if (file.bad())
	{
		Debug::LogError(eastl::string("Can't open binary shader file ") + adjustedName + eastl::string(". Try recompiling the shaders."));
	}
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<char> buffer(size);
	file.read(buffer.data(), size);
	file.close();
	pShaders_.insert({name, activeGame_->GetRender()->GetDevice()->createShader(
								nvrhi::ShaderDesc(nvrhi::ShaderType::Pixel),
								&(buffer[0]), buffer.size() * sizeof(char))});
	return pShaders_[name];
}

nvrhi::ShaderHandle D3E::ShaderFactory::AddGeometryShader(const eastl::string& name,
                                                          const eastl::string& fileName,
                                                          const eastl::string& entryPoint)
{
	auto adjustedName = GetBinaryShaderFileName(fileName, entryPoint);
	std::ifstream file(adjustedName.c_str(), std::ios::binary | std::ios::ate);
	if (file.bad())
	{
		Debug::LogError(eastl::string("Can't open binary shader file ") + adjustedName + eastl::string(". Try recompiling the shaders."));
	}
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<char> buffer(size);
	file.read(buffer.data(), size);
	file.close();
	gShaders_.insert({name, activeGame_->GetRender()->GetDevice()->createShader(
								nvrhi::ShaderDesc(nvrhi::ShaderType::Geometry),
								&(buffer[0]), buffer.size() * sizeof(char))});
	return gShaders_[name];
}

nvrhi::ShaderHandle D3E::ShaderFactory::GetVertexShader(const eastl::string& name)
{
	return vShaders_[name];
}

nvrhi::ShaderHandle D3E::ShaderFactory::GetPixelShader(const eastl::string& name)
{
	return pShaders_[name];
}

nvrhi::ShaderHandle D3E::ShaderFactory::GetGeometryShader(const eastl::string& name)
{
	return gShaders_[name];
}

eastl::string D3E::ShaderFactory::GetBinaryShaderFileName(const eastl::string& fileName,
                                                          const eastl::string& entryPoint)
{
	eastl::string adjustedName = fileName;
	size_t pos = adjustedName.find(".hlsl");
	if (pos != eastl::string::npos)
	{
		adjustedName.erase(pos, 5);
	}

	adjustedName += "_" + entryPoint;

	adjustedName = eastl::string(std::filesystem::current_path().string().c_str()) + "\\Shaders\\" + adjustedName + ".dxil";

	return adjustedName;
}

nvrhi::ShaderHandle D3E::ShaderFactory::AddComputeShader(const eastl::string& name,
                                                         const eastl::string& fileName,
                                                         const eastl::string& entryPoint)
{
	auto adjustedName = GetBinaryShaderFileName(fileName, entryPoint);
	std::ifstream file(adjustedName.c_str(), std::ios::binary | std::ios::ate);
	if (file.bad())
	{
		Debug::LogError(eastl::string("Can't open binary shader file ") + adjustedName + eastl::string(". Try recompiling the shaders."));
	}
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<char> buffer(size);
	file.read(buffer.data(), size);
	file.close();
	cShaders_.insert({name, activeGame_->GetRender()->GetDevice()->createShader(
								nvrhi::ShaderDesc(nvrhi::ShaderType::Compute),
								&(buffer[0]), buffer.size() * sizeof(char))});
	return cShaders_[name];
}

nvrhi::ShaderHandle D3E::ShaderFactory::GetComputeShader(const eastl::string& name)
{
	return cShaders_[name];
}
