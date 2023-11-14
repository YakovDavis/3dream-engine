#include "ShaderFactory.h"

#include <fstream>
#include "D3E/Game.h"
#include "render/GameRender.h"
#include <filesystem>
#include "D3E/Debug.h"

bool D3E::ShaderFactory::isInitialized_ = false;
D3E::Game* D3E::ShaderFactory::activeGame_;
eastl::unordered_map<eastl::string, nvrhi::InputLayoutHandle> D3E::ShaderFactory::iLayouts_ {};
eastl::unordered_map<eastl::string, nvrhi::BindingLayoutHandle> D3E::ShaderFactory::bLayouts_ {};
eastl::unordered_map<eastl::string, nvrhi::BindingSetHandle> D3E::ShaderFactory::bSets_ {};
eastl::unordered_map<eastl::string, nvrhi::ShaderHandle> D3E::ShaderFactory::vShaders_ {};
eastl::unordered_map<eastl::string, nvrhi::ShaderHandle> D3E::ShaderFactory::pShaders_ {};
eastl::unordered_map<eastl::string, nvrhi::ShaderHandle> D3E::ShaderFactory::gShaders_ {};
eastl::unordered_map<eastl::string, nvrhi::ShaderHandle> D3E::ShaderFactory::cShaders_ {};
eastl::unordered_map<eastl::string, nvrhi::GraphicsPipelineHandle > D3E::ShaderFactory::gPipelines_ {};

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

const nvrhi::ShaderHandle& D3E::ShaderFactory::AddVertexShader(const eastl::string& name,
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

const nvrhi::ShaderHandle& D3E::ShaderFactory::AddPixelShader(const eastl::string& name,
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

const nvrhi::ShaderHandle& D3E::ShaderFactory::AddGeometryShader(const eastl::string& name,
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

const nvrhi::ShaderHandle& D3E::ShaderFactory::GetVertexShader(const eastl::string& name)
{
	if (vShaders_.find(name) == vShaders_.end())
	{
		Debug::LogError("[ShaderFactory] Vertex shader not found");
	}
	return vShaders_[name];
}

const nvrhi::ShaderHandle& D3E::ShaderFactory::GetPixelShader(const eastl::string& name)
{
	if (pShaders_.find(name) == pShaders_.end())
	{
		Debug::LogError("[ShaderFactory] Pixel shader not found");
	}
	return pShaders_[name];
}

const nvrhi::ShaderHandle& D3E::ShaderFactory::GetGeometryShader(const eastl::string& name)
{
	if (gShaders_.find(name) == gShaders_.end())
	{
		Debug::LogError("[ShaderFactory] Geometry shader not found");
	}
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

	if (entryPoint != "main")
	{
		adjustedName += "_" + entryPoint;
	}

	adjustedName = eastl::string(std::filesystem::current_path().string().c_str()) + "\\Shaders\\" + adjustedName + ".dxil";

	return adjustedName;
}

const nvrhi::ShaderHandle& D3E::ShaderFactory::AddComputeShader(const eastl::string& name,
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

const nvrhi::ShaderHandle& D3E::ShaderFactory::GetComputeShader(const eastl::string& name)
{
	if (cShaders_.find(name) == cShaders_.end())
	{
		Debug::LogError("[ShaderFactory] Compute shader not found");
	}
	return cShaders_[name];
}

const nvrhi::InputLayoutHandle& D3E::ShaderFactory::AddInputLayout(const eastl::string& name,
                                                            nvrhi::VertexAttributeDesc* desc,
                                                            uint32_t count,
                                                            const nvrhi::ShaderHandle& vs)
{
	iLayouts_.insert({name, activeGame_->GetRender()->GetDevice()->createInputLayout(desc, uint32_t(count), vs)});
	return iLayouts_[name];
}

const nvrhi::InputLayoutHandle& D3E::ShaderFactory::GetInputLayout(const eastl::string& name)
{
	if (iLayouts_.find(name) == iLayouts_.end())
	{
		Debug::LogError("[ShaderFactory] Input layout not found");
	}
	return iLayouts_[name];
}

const nvrhi::BindingLayoutHandle& D3E::ShaderFactory::AddBindingLayout(const eastl::string& name,
                                     const nvrhi::BindingLayoutDesc& desc)
{
	bLayouts_.insert({name, activeGame_->GetRender()->GetDevice()->createBindingLayout(desc)});
	return bLayouts_[name];
}

const nvrhi::BindingLayoutHandle& D3E::ShaderFactory::GetBindingLayout(const eastl::string& name)
{
	if (bLayouts_.find(name) == bLayouts_.end())
	{
		Debug::LogError("[ShaderFactory] Binding layout not found");
	}
	return bLayouts_[name];
}

const nvrhi::GraphicsPipelineHandle& D3E::ShaderFactory::AddGraphicsPipeline(const eastl::string& name,
                                                                            const nvrhi::GraphicsPipelineDesc& desc,
                                                                            const nvrhi::FramebufferHandle& fb)
{
	gPipelines_.insert({name, activeGame_->GetRender()->GetDevice()->createGraphicsPipeline(desc, fb)});
	return gPipelines_[name];
}

const nvrhi::GraphicsPipelineHandle& D3E::ShaderFactory::GetGraphicsPipeline(const eastl::string& name)
{
	if (gPipelines_.find(name) == gPipelines_.end())
	{
		Debug::LogError("[ShaderFactory] Graphics pipeline not found");
	}
	return gPipelines_[name];
}

const nvrhi::BindingSetHandle& D3E::ShaderFactory::GetBindingSet(const eastl::string& name)
{
	if (bSets_.find(name) == bSets_.end())
	{
		Debug::LogError("[ShaderFactory] Binding layout not found");
	}
	return bSets_[name];
}

const nvrhi::BindingSetHandle& D3E::ShaderFactory::AddBindingSet(const eastl::string& name,
                                  const nvrhi::BindingSetDesc& desc, const eastl::string& bLayoutName)
{
	bSets_.insert({name, activeGame_->GetRender()->GetDevice()->createBindingSet(desc, GetBindingLayout(bLayoutName))});
	return bSets_[name];
}
