#include "Editor.h"

#include "nvrhi/nvrhi.h"
#include "render/DisplayWin32.h"

D3E::Editor* D3E::Editor::instance_;

D3E::Editor::Editor(const nvrhi::DeviceHandle& device, eastl::shared_ptr<Display> display) : display_{display}
{
	auto displayWin32 = dynamic_cast<DisplayWin32*>(display_.get());

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(displayWin32->hWnd);
	imGuiNvrhi_.init(device);
}

D3E::Editor* D3E::Editor::Init(const nvrhi::DeviceHandle& device, eastl::shared_ptr<Display> display)
{
	if(instance_ == nullptr)
	{
		instance_ = new Editor(device, display);
	}

	return instance_;
}

void D3E::Editor::Render(nvrhi::IFramebuffer* currentFramebuffer)
{
	ImGui_ImplWin32_NewFrame();

	ImGui::ShowDemoWindow();
	ImGui::Render();

	imGuiNvrhi_.render(currentFramebuffer);
}
void D3E::Editor::Update(float deltaTime)
{
	int w, h;
	float scaleX, scaleY;

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(float(display_->ClientWidth), float(display_->ClientHeight));
	//io.DisplayFramebufferScale.x = scaleX;
	//io.DisplayFramebufferScale.y = scaleY;

	imGuiNvrhi_.beginFrame(deltaTime);
}

