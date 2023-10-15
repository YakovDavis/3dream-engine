#include "GameRender.h"
#include "nvrhi/d3d12.h" // TODO: vulkan support

void D3E::GameRender::Init()
{
	nvrhi::d3d12::DeviceDesc deviceDesc;
	//deviceDesc.errorCB = g_MyMessageCallback;
	deviceDesc.pDevice = d3d12Device;
	deviceDesc.pGraphicsCommandQueue = d3d12GraphicsCommandQueue;

	nvrhi::DeviceHandle nvrhiDevice = nvrhi::d3d12::createDevice(deviceDesc);
}

void D3E::GameRender::DestroyResources()
{
}
