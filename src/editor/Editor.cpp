#include "Editor.h"

#include "D3E/Debug.h"
#include "nvrhi/nvrhi.h"
#include "render/DisplayWin32.h"
#include "editor/EditorUtils.h"

#include <sstream>
#include <cstring>

D3E::Editor* D3E::Editor::instance_;

D3E::Editor::Editor(const nvrhi::DeviceHandle& device, eastl::shared_ptr<Display> display) : display_{display}
{
	auto displayWin32 = dynamic_cast<DisplayWin32*>(display_.get());

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	SetStyle();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags = ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags = ImGuiConfigFlags_::ImGuiConfigFlags_ViewportsEnable;

	ImGui_ImplWin32_Init(displayWin32->hWnd);
	imGuiNvrhi_.init(device);
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
	DrawHeader();
	DrawPlay();
	DrawHierarchy();
	DrawInspector();
	DrawContentBrowser();

	ImGui::Render();
	imGuiNvrhi_.render(currentFramebuffer);

	// not sure if it's necessary
	if(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		auto context = ImGui::GetCurrentContext();
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

//	ImGuiIO& io = ImGui::GetIO();
//	static float f = 0.0f;
//	static int counter = 0;
//
//	ImGui::Begin("Content Browser");                          // Create a window called "Hello, world!" and append into it.
//
//	ImGui::Text("This is some useful text.");
//
//	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
//	ImGui::ColorEdit3("clear color", (float*)&color_); // Edit 3 floats representing a color
//
//	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
//		counter++;
//	ImGui::SameLine();
//	ImGui::Text("counter = %d", counter);
//
//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
//	ImGui::End();
}
