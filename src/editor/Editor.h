#pragma once

#include "D3E/Debug.h"
#include "EASTL/shared_ptr.h"
#include "EASTL/string.h"
#include "EditorConsole.h"
#include "EditorContentBrowser.h"
#include "imgui.h"
#include "imgui_backend/imgui_nvrhi.h"
#include "imgui_impl_win32.h"
#include "render/Display.h"

namespace D3E
{
	class Game;

	class Editor
	{
	private:
		static Editor* instance_;
	public:
		static Editor* Init(const nvrhi::DeviceHandle& device, eastl::shared_ptr<Display> display, Game *game);
		static void PrintConsoleMessage(const eastl::string& str, D3E::Debug::TextColor color);

	private:
		Game *game_;
		ImGui_NVRHI imGuiNvrhi_;
		eastl::shared_ptr<Display> display_;
		float color_[4] = {0.f, 0.f, 0.f, 0.f};
		EditorConsole *editorConsole_;
		EditorContentBrowser *editorContentBrowser_;

	private:
		Editor(const nvrhi::DeviceHandle& device, eastl::shared_ptr<Display> display, Game *game);
		void SetStyle();
		void PrintConsoleMessageInternal(const eastl::string& str, D3E::Debug::TextColor color);

		void DrawViewport(nvrhi::IFramebuffer* currentFramebuffer);
		void DrawHeader();
		void DrawPlay();
		void DrawHierarchy();
		void DrawInspector();
	public:
		void BeginDraw(float deltaTime);
		void EndDraw(nvrhi::IFramebuffer* currentFramebuffer);
		void Release();
	};
}