#pragma once

#include "D3E/Debug.h"
#include "EASTL/shared_ptr.h"
#include "EASTL/string.h"
#include "EditorConsole.h"
#include "EditorContentBrowser.h"
#include "MaterialEditor.h"
#include "EditorUtils.h"
#include "imgui.h"
#include "imgui_backend/imgui_nvrhi.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
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
		static Editor* Get() { return instance_; }

		bool IsMouseOnViewport();
		void GetMousePositionInViewport(int& mouseX, int& mouseY);

	private:
		Game *game_;
		ImGui_NVRHI imGuiNvrhi_;
		eastl::shared_ptr<Display> display_;
		float color_[4] = {0.f, 0.f, 0.f, 0.f};
		EditorConsole *editorConsole_;
		EditorContentBrowser* editorContentBrowser_;
		MaterialEditor* materialEditor_;
		bool hoveringOnViewport = false;
		bool viewportFocused = false;
		bool usingGizmo = false;
		ImRect viewportInnerRect;
		ImVec2 viewportDimensions;
		bool lmbDownLastFrame = false;

	private:
		Editor(const nvrhi::DeviceHandle& device, eastl::shared_ptr<Display> display, Game *game);
		void SetStyle();
		void PrintConsoleMessageInternal(const eastl::string& str, D3E::Debug::TextColor color);

		struct HierarchiNode
		{
			EditorObjectInfo info;
			eastl::vector<HierarchiNode*> children;
		};

		void ShowEditorApp(bool* p_open);

		void DrawViewport(nvrhi::IFramebuffer* gameFramebuffer);
		void DrawHeader();
		void DrawPlay();
		void DrawHierarchy();
		void DrawHierarchyNode(HierarchiNode* node, String& uuidSelected);
		void DrawInspector();
		void DrawGizmo();
		void DrawTransformEdit();

		void AlignForWidth(float width, float alignment = 0.5f);
	public:
		void BeginDraw(float deltaTime);
		void EndDraw(nvrhi::IFramebuffer* currentFramebuffer, nvrhi::IFramebuffer* gameFramebuffer);
		void Release();

		friend class EditorContentBrowser;
		friend class MaterialEditor;
	};
}