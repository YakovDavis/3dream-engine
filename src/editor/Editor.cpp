#include "Editor.h"

#include "D3E/Debug.h"
#include "nvrhi/nvrhi.h"
#include "render/DisplayWin32.h"
#include "editor/EditorUtils.h"

#include <sstream>
#include <cstring>

D3E::Editor* D3E::Editor::instance_;

void ShowExampleAppDockSpace(bool* p_open)
{
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockSpace_flags = ImGuiDockNodeFlags_None;
	dockSpace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockSpace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockSpace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", p_open, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// Submit the DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockSpace_flags);
	}


	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows,
			// which we can't undo at the moment without finer window depth/z control.
			ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
			ImGui::MenuItem("Padding", NULL, &opt_padding);
			ImGui::Separator();

			if (ImGui::MenuItem("Flag: NoDockingOverCentralNode", "", (dockSpace_flags & ImGuiDockNodeFlags_NoDockingOverCentralNode) != 0)) { dockSpace_flags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode; }
			if (ImGui::MenuItem("Flag: NoDockingSplit",         "", (dockSpace_flags & ImGuiDockNodeFlags_NoDockingSplit) != 0))             { dockSpace_flags ^= ImGuiDockNodeFlags_NoDockingSplit; }
			if (ImGui::MenuItem("Flag: NoUndocking",            "", (dockSpace_flags & ImGuiDockNodeFlags_NoUndocking) != 0))                { dockSpace_flags ^= ImGuiDockNodeFlags_NoUndocking; }
			if (ImGui::MenuItem("Flag: NoResize",               "", (dockSpace_flags & ImGuiDockNodeFlags_NoResize) != 0))                   { dockSpace_flags ^= ImGuiDockNodeFlags_NoResize; }
			if (ImGui::MenuItem("Flag: AutoHideTabBar",         "", (dockSpace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))             { dockSpace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
			if (ImGui::MenuItem("Flag: PassthruCentralNode",    "", (dockSpace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockSpace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
			ImGui::Separator();

			if (ImGui::MenuItem("Close", NULL, false, p_open != NULL))
				*p_open = false;
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::End();
}

D3E::Editor::Editor(const nvrhi::DeviceHandle& device, eastl::shared_ptr<Display> display) : display_{display}
{
	auto displayWin32 = dynamic_cast<DisplayWin32*>(display_.get());

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	SetStyle();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_ViewportsEnable;

	ImGui_ImplWin32_Init(displayWin32->hWnd);
	imGuiNvrhi_.init(device);

	editorConsole_ = new EditorConsole();
}

void D3E::Editor::SetStyle()
{
	ImGui::StyleColorsDark();
	auto style = ImGui::GetStyle();
	style.WindowRounding = 0.5f;
	style.Colors[ImGuiCol_WindowBg].w = 1.0f;
}

D3E::Editor* D3E::Editor::Init(const nvrhi::DeviceHandle& device, eastl::shared_ptr<Display> display)
{
	if(instance_ == nullptr)
	{
		instance_ = new Editor(device, display);
		Debug::LogMessage("[ImGUI] Init");
	}

	return instance_;
}

void D3E::Editor::BeginDraw(float deltaTime)
{
	auto displaySize = ImVec2(float(display_->ClientWidth), float(display_->ClientHeight));
	ImGui_ImplWin32_NewFrame();
	imGuiNvrhi_.beginFrame(deltaTime, displaySize);
}

void D3E::Editor::EndDraw(nvrhi::IFramebuffer* currentFramebuffer)
{
	bool show;
	ShowExampleAppDockSpace(&show);

	//DrawViewport(currentFramebuffer);
	DrawHeader();
	DrawPlay();
	DrawHierarchy();
	DrawInspector();
	DrawContentBrowser();
	editorConsole_->Draw();

	ImGui::Render();
	imGuiNvrhi_.render(currentFramebuffer);

	if(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault(nullptr, imGuiNvrhi_.m_commandList);
	}
}

void D3E::Editor::Release()
{
	if(ImGui::GetCurrentContext())
	{
		ImGui::DestroyContext();
		ImGui_ImplWin32_Shutdown();
	}
}

void D3E::Editor::DrawViewport(nvrhi::IFramebuffer* currentFramebuffer)
{
	ImGui::Begin("Viewport");
	auto texture = currentFramebuffer->getDesc().colorAttachments[0].texture;
	ImGui::Image(texture, ImVec2{1280, 720}, ImVec2{0, 1}, ImVec2{1, 0});
	ImGui::End();
}
void D3E::Editor::DrawHeader()
{
}
void D3E::Editor::DrawPlay()
{
}
void D3E::Editor::DrawHierarchy()
{
	ImGui::Begin("Hierarchy");
	auto objects = EditorUtils::ListActiveObjects();
	for(int i = objects.size() - 1; i >= 0 ; i--)
	{
		if(i > 0 && strcmp(objects[i].name.c_str(), objects[i - 1].name.c_str()) == 0)
		{
			continue;
		}
		ImGui::Text(objects[i].name.c_str());
	}
	ImGui::End();
}
void D3E::Editor::DrawInspector()
{
	ImGui::Begin("Inspector");
	ImGui::Text("Here will be some useful information about object");
	ImGui::End();
}
void D3E::Editor::DrawContentBrowser()
{
	ImGui::Begin("Content Browser");
	ImGui::Text("Here will be some useful information about files in the project directory");
	ImGui::End();
}