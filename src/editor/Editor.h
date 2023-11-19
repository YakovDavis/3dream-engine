#pragma once

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_backend/imgui_nvrhi.h"
#include "EASTL/shared_ptr.h"
#include "render/Display.h"

namespace D3E
{
	class Editor
	{
	private:
		ImGui_NVRHI imGuiNvrhi_;
		eastl::shared_ptr<Display> display_;
		Editor(const nvrhi::DeviceHandle& device, eastl::shared_ptr<Display> display);
		static Editor* instance_;
	public:
		static Editor* Init(const nvrhi::DeviceHandle& device, eastl::shared_ptr<Display> display);
		void Render(nvrhi::IFramebuffer* currentFramebuffer);
		void Update(float deltaTime);
	};
}