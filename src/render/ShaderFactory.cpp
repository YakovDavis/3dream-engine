#include "ShaderFactory.h"

#include "D3E/CommonCpp.h"
#include "D3E/Debug.h"
#include "D3E/Game.h"
#include "render/GameRender.h"

#include <filesystem>
#include <fstream>

bool D3E::ShaderFactory::isInitialized_ = false;
D3E::Game* D3E::ShaderFactory::activeGame_;
eastl::unordered_map<D3E::String, nvrhi::InputLayoutHandle> D3E::ShaderFactory::iLayouts_ {};
eastl::unordered_map<D3E::String, nvrhi::BindingLayoutHandle> D3E::ShaderFactory::bLayouts_ {};
eastl::unordered_map<D3E::String, nvrhi::BindingSetHandle> D3E::ShaderFactory::bSetsV_ {};
eastl::unordered_map<D3E::String, nvrhi::BindingSetHandle> D3E::ShaderFactory::bSetsP_ {};
eastl::unordered_map<D3E::String, nvrhi::BindingSetHandle> D3E::ShaderFactory::bSetsC_ {};
eastl::unordered_map<D3E::String, nvrhi::ShaderHandle> D3E::ShaderFactory::vShaders_ {};
eastl::unordered_map<D3E::String, nvrhi::ShaderHandle> D3E::ShaderFactory::pShaders_ {};
eastl::unordered_map<D3E::String, nvrhi::ShaderHandle> D3E::ShaderFactory::gShaders_ {};
eastl::unordered_map<D3E::String, nvrhi::ShaderHandle> D3E::ShaderFactory::cShaders_ {};
eastl::unordered_map<D3E::String, nvrhi::GraphicsPipelineHandle > D3E::ShaderFactory::gPipelines_ {};
eastl::unordered_map<D3E::String, nvrhi::ComputePipelineHandle > D3E::ShaderFactory::cPipelines_ {};

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

const nvrhi::ShaderHandle& D3E::ShaderFactory::AddVertexShader(const String& name,
                                    					const String& fileName,
                                                        const String& entryPoint)
{
	auto adjustedName = GetBinaryShaderFileName(fileName, entryPoint);
	std::ifstream file(adjustedName.c_str(), std::ios::binary | std::ios::ate);
	if (!file.good())
	{
		Debug::LogError(String("Can't open binary shader file ") + adjustedName + String(". Try recompiling the shaders."));
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

const nvrhi::ShaderHandle& D3E::ShaderFactory::AddPixelShader(const String& name,
                                                       const String& fileName,
                                                       const String& entryPoint)
{
	auto adjustedName = GetBinaryShaderFileName(fileName, entryPoint);
	std::ifstream file(adjustedName.c_str(), std::ios::binary | std::ios::ate);
	if (file.bad())
	{
		Debug::LogError(String("Can't open binary shader file ") + adjustedName + String(". Try recompiling the shaders."));
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

const nvrhi::ShaderHandle& D3E::ShaderFactory::AddGeometryShader(const String& name,
                                                          const String& fileName,
                                                          const String& entryPoint)
{
	auto adjustedName = GetBinaryShaderFileName(fileName, entryPoint);
	std::ifstream file(adjustedName.c_str(), std::ios::binary | std::ios::ate);
	if (file.bad())
	{
		Debug::LogError(String("Can't open binary shader file ") + adjustedName + String(". Try recompiling the shaders."));
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

const nvrhi::ShaderHandle& D3E::ShaderFactory::GetVertexShader(const String& name)
{
	if (vShaders_.find(name) == vShaders_.end())
	{
		Debug::LogError("[ShaderFactory] Vertex shader not found");
	}
	return vShaders_[name];
}

const nvrhi::ShaderHandle& D3E::ShaderFactory::GetPixelShader(const String& name)
{
	if (pShaders_.find(name) == pShaders_.end())
	{
		Debug::LogError("[ShaderFactory] Pixel shader not found");
	}
	return pShaders_[name];
}

const nvrhi::ShaderHandle& D3E::ShaderFactory::GetGeometryShader(const String& name)
{
	if (gShaders_.find(name) == gShaders_.end())
	{
		Debug::LogError("[ShaderFactory] Geometry shader not found");
	}
	return gShaders_[name];
}

D3E::String D3E::ShaderFactory::GetBinaryShaderFileName(const String& fileName,
                                                          const String& entryPoint)
{
	String adjustedName = fileName;
	size_t pos = adjustedName.find(".hlsl");
	if (pos != String::npos)
	{
		adjustedName.erase(pos, 5);
	}

	if (entryPoint != "main")
	{
		adjustedName += "_" + entryPoint;
	}

	adjustedName = String(std::filesystem::current_path().string().c_str()) + "\\Shaders\\" + adjustedName + ".dxil";

	return adjustedName;
}

const nvrhi::ShaderHandle& D3E::ShaderFactory::AddComputeShader(const String& name,
                                                         const String& fileName,
                                                         const String& entryPoint)
{
	auto adjustedName = GetBinaryShaderFileName(fileName, entryPoint);
	std::ifstream file(adjustedName.c_str(), std::ios::binary | std::ios::ate);
	if (file.bad())
	{
		Debug::LogError(String("Can't open binary shader file ") + adjustedName + String(". Try recompiling the shaders."));
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

const nvrhi::ShaderHandle& D3E::ShaderFactory::GetComputeShader(const String& name)
{
	if (cShaders_.find(name) == cShaders_.end())
	{
		Debug::LogError("[ShaderFactory] Compute shader not found");
	}
	return cShaders_[name];
}

const nvrhi::InputLayoutHandle& D3E::ShaderFactory::AddInputLayout(const String& name,
                                                            nvrhi::VertexAttributeDesc* desc,
                                                            uint32_t count,
                                                            const nvrhi::ShaderHandle& vs)
{
	iLayouts_.insert({name, activeGame_->GetRender()->GetDevice()->createInputLayout(desc, uint32_t(count), vs)});
	return iLayouts_[name];
}

const nvrhi::InputLayoutHandle& D3E::ShaderFactory::GetInputLayout(const String& name)
{
	if (iLayouts_.find(name) == iLayouts_.end())
	{
		Debug::LogError("[ShaderFactory] Input layout not found");
	}
	return iLayouts_[name];
}

const nvrhi::BindingLayoutHandle& D3E::ShaderFactory::AddBindingLayout(const String& name,
                                     const nvrhi::BindingLayoutDesc& desc)
{
	bLayouts_.insert({name, activeGame_->GetRender()->GetDevice()->createBindingLayout(desc)});
	return bLayouts_[name];
}

const nvrhi::BindingLayoutHandle& D3E::ShaderFactory::GetBindingLayout(const String& name)
{
	if (bLayouts_.find(name) == bLayouts_.end())
	{
		Debug::LogError("[ShaderFactory] Binding layout not found");
	}
	return bLayouts_[name];
}

const nvrhi::GraphicsPipelineHandle& D3E::ShaderFactory::AddGraphicsPipeline(const String& name,
                                                                            const nvrhi::GraphicsPipelineDesc& desc,
                                                                            const nvrhi::FramebufferHandle& fb)
{
	gPipelines_.insert({name, activeGame_->GetRender()->GetDevice()->createGraphicsPipeline(desc, fb)});
	return gPipelines_[name];
}

const nvrhi::GraphicsPipelineHandle& D3E::ShaderFactory::GetGraphicsPipeline(const String& name)
{
	if (gPipelines_.find(name) == gPipelines_.end())
	{
		Debug::LogError("[ShaderFactory] Graphics pipeline not found");
	}
	return gPipelines_[name];
}

const nvrhi::BindingSetHandle& D3E::ShaderFactory::GetBindingSetV(const String& name)
{
	if (bSetsV_.find(name) == bSetsV_.end())
	{
		Debug::LogError("[ShaderFactory] Binding set not found");
	}
	return bSetsV_[name];
}

const nvrhi::BindingSetHandle& D3E::ShaderFactory::GetBindingSetP(const String& name)
{
	if (bSetsP_.find(name) == bSetsP_.end())
	{
		Debug::LogError("[ShaderFactory] Binding set not found");
	}
	return bSetsP_[name];
}

const nvrhi::BindingSetHandle& D3E::ShaderFactory::AddBindingSetV(const String& name,
                                  const nvrhi::BindingSetDesc& desc, const String& bLayoutName)
{
	bSetsV_.insert({name, activeGame_->GetRender()->GetDevice()->createBindingSet(desc, GetBindingLayout(bLayoutName))});
	return bSetsV_[name];
}

const nvrhi::BindingSetHandle& D3E::ShaderFactory::AddBindingSetP(const String& name,
                                                                  const nvrhi::BindingSetDesc& desc, const String& bLayoutName)
{
	bSetsP_.insert({name, activeGame_->GetRender()->GetDevice()->createBindingSet(desc, GetBindingLayout(bLayoutName))});
	return bSetsP_[name];
}

const nvrhi::ComputePipelineHandle& D3E::ShaderFactory::AddComputePipeline(const D3E::String& name,
                                       const nvrhi::ComputePipelineDesc& desc)
{
	cPipelines_.insert({name, activeGame_->GetRender()->GetDevice()->createComputePipeline(desc)});
	return cPipelines_[name];
}

const nvrhi::ComputePipelineHandle& D3E::ShaderFactory::GetComputePipeline(const D3E::String& name)
{
	if (cPipelines_.find(name) == cPipelines_.end())
	{
		Debug::LogError("[ShaderFactory] Compute pipeline not found");
	}
	return cPipelines_[name];
}

const nvrhi::BindingSetHandle& D3E::ShaderFactory::AddBindingSetC(const D3E::String& name,
                                   const nvrhi::BindingSetDesc& desc,
                                   const D3E::String& bLayoutName)
{
	bSetsC_.insert({name, activeGame_->GetRender()->GetDevice()->createBindingSet(desc, GetBindingLayout(bLayoutName))});
	return bSetsC_[name];
}

const nvrhi::BindingSetHandle& D3E::ShaderFactory::GetBindingSetC(const D3E::String& name)
{
	if (bSetsC_.find(name) == bSetsC_.end())
	{
		Debug::LogError("[ShaderFactory] Binding set not found");
	}
	return bSetsC_[name];
}
