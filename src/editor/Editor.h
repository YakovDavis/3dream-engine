#pragma once

#include "EASTL/shared_ptr.h"
#include "EditorConsole.h"
#include "imgui.h"
#include "imgui_backend/imgui_nvrhi.h"
#include "imgui_impl_win32.h"
#include "render/Display.h"

namespace D3E
{
	class Editor
	{
	private:
		static Editor* instance_;
	public:
		static Editor* Init(const nvrhi::DeviceHandle& device, eastl::shared_ptr<Display> display);

	private:
		ImGui_NVRHI imGuiNvrhi_;
		eastl::shared_ptr<Display> display_;
		float color_[4] = {0.f, 0.f, 0.f, 0.f};
		EditorConsole *editorConsole_;
	private:
		Editor(const nvrhi::DeviceHandle& device, eastl::shared_ptr<Display> display);
		void SetStyle();

		void DrawViewport(nvrhi::IFramebuffer* currentFramebuffer);
		void DrawHeader();
		void DrawPlay();
		void DrawHierarchy();
		void DrawInspector();
		void DrawContentBrowser();
	public:
		void BeginDraw(float deltaTime);
		void EndDraw(nvrhi::IFramebuffer* currentFramebuffer);
		void Release();
	};
}