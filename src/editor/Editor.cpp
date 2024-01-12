#include "Editor.h"

#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Components/TransformComponent.h"
#include "D3E/Debug.h"
#include "D3E/Game.h"
#include "ImGuizmo.h"
#include "assetmng/TextureFactory.h"
#include "core/EngineState.h"
#include "imgui_internal.h"
#include "input/InputDevice.h"
#include "nvrhi/nvrhi.h"
#include "render/CameraUtils.h"
#include "render/DisplayWin32.h"

D3E::Editor* D3E::Editor::instance_;

bool useWindow = false;
float camDistance = 8.f;
static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
static bool useSnap = false;
static float snap[3] = { 1.f, 1.f, 1.f };
static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
static bool boundSizing = false;
static bool boundSizingSnap = false;

void ShowExampleAppDockSpace(bool* p_open)
{
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockSpace_flags =
		ImGuiDockNodeFlags_PassthruCentralNode;

	ImGuiWindowFlags window_flags =
		ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar |
		                ImGuiWindowFlags_NoCollapse |
		                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus |
		                ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockSpace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will
	// render our background and handle the pass-thru hole, so we ask Begin() to
	// not render a background.
	if (dockSpace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window
	// is collapsed). This is because we want to keep our DockSpace() active. If
	// a DockSpace() is inactive, all active windows docked into it will lose
	// their parent and become undocked. We cannot preserve the docking
	// relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in
	// limbo and never being visible.
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
			/*
			// Disabling fullscreen would allow the window to be moved to the
			// front of other windows, which we can't undo at the moment without
			// finer window depth/z control.
			ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
			ImGui::MenuItem("Padding", NULL, &opt_padding);
			ImGui::Separator();

			if (ImGui::MenuItem("Flag: NoDockingOverCentralNode", "",
			                    (dockSpace_flags &
			                     ImGuiDockNodeFlags_NoDockingOverCentralNode) !=
			                        0))
			{
				dockSpace_flags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode;
			}
			if (ImGui::MenuItem(
					"Flag: NoDockingSplit", "",
					(dockSpace_flags & ImGuiDockNodeFlags_NoDockingSplit) != 0))
			{
				dockSpace_flags ^= ImGuiDockNodeFlags_NoDockingSplit;
			}
			if (ImGui::MenuItem(
					"Flag: NoUndocking", "",
					(dockSpace_flags & ImGuiDockNodeFlags_NoUndocking) != 0))
			{
				dockSpace_flags ^= ImGuiDockNodeFlags_NoUndocking;
			}
			if (ImGui::MenuItem(
					"Flag: NoResize", "",
					(dockSpace_flags & ImGuiDockNodeFlags_NoResize) != 0))
			{
				dockSpace_flags ^= ImGuiDockNodeFlags_NoResize;
			}
			if (ImGui::MenuItem(
					"Flag: AutoHideTabBar", "",
					(dockSpace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))
			{
				dockSpace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
			}
			if (ImGui::MenuItem("Flag: PassthruCentralNode", "",
			                    (dockSpace_flags &
			                     ImGuiDockNodeFlags_PassthruCentralNode) != 0,
			                    opt_fullscreen))
			{
				dockSpace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
			}
			ImGui::Separator();

			*/

			if (ImGui::MenuItem("Close", NULL, false, p_open != NULL))
				*p_open = false;
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::End();
}

D3E::Editor::Editor(const nvrhi::DeviceHandle& device,
                    eastl::shared_ptr<Display> display, Game* game)
	: display_{display}, game_{game}
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
	editorContentBrowser_ = new EditorContentBrowser(this);
}

void D3E::Editor::SetStyle()
{
	ImGui::StyleColorsDark();
	auto style = ImGui::GetStyle();
	style.WindowRounding = 0.5f;
	style.Colors[ImGuiCol_WindowBg].w = 1.0f;
}

D3E::Editor* D3E::Editor::Init(const nvrhi::DeviceHandle& device,
                               eastl::shared_ptr<Display> display, Game* game)
{
	if (instance_ == nullptr)
	{
		instance_ = new Editor(device, display, game);
		Debug::LogMessage("[ImGUI] Init");
	}

	return instance_;
}

void D3E::Editor::BeginDraw(float deltaTime)
{
	auto displaySize =
		ImVec2(float(display_->ClientWidth), float(display_->ClientHeight));
	ImGui_ImplWin32_NewFrame();
	imGuiNvrhi_.beginFrame(deltaTime, displaySize);
}

void D3E::Editor::EndDraw(nvrhi::IFramebuffer* currentFramebuffer, nvrhi::IFramebuffer* gameFramebuffer)
{
	bool show;
	ShowExampleAppDockSpace(&show);

	if (!game_->GetSelectedUuids().empty())
	{
		DrawTransformEdit();
	}
	DrawViewport(gameFramebuffer);
	DrawHeader();
	DrawPlay();
	DrawHierarchy();
	DrawInspector();
	editorConsole_->Draw();
	editorContentBrowser_->Draw();

	ImGui::Render();
	imGuiNvrhi_.render(currentFramebuffer);

	if (usingGizmo)
	{
		Game::MouseLockedByImGui = true;
	}
	else if (hoveringOnViewport || viewportFocused)
	{
		Game::MouseLockedByImGui = false;
	}
	else
	{
		Game::MouseLockedByImGui = ImGui::GetIO().WantCaptureMouse;
	}

	Game::KeyboardLockedByImGui = ImGui::GetIO().WantCaptureKeyboard;

	// not sure if it's necessary
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault(nullptr, imGuiNvrhi_.m_commandList);
	}
}

void D3E::Editor::Release()
{
	if (ImGui::GetCurrentContext())
	{
		ImGui::DestroyContext();
	}
}

void D3E::Editor::PrintConsoleMessage(const eastl::string& str,
                                      D3E::Debug::TextColor color)
{
	if (instance_)
	{
		instance_->PrintConsoleMessageInternal(str, color);
	}
}

void D3E::Editor::PrintConsoleMessageInternal(const eastl::string& str,
                                              D3E::Debug::TextColor color)
{
	editorConsole_->PrintMessage(str, color);
}

void D3E::Editor::DrawViewport(nvrhi::IFramebuffer* gameFramebuffer)
{
	ImGuiIO& io = ImGui::GetIO();
	float viewManipulateRight = io.DisplaySize.x;
	float viewManipulateTop = 0;
	static ImGuiWindowFlags windowFlags = 0;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(0.35f, 0.3f, 0.3f));
	ImGui::Begin("Viewport", 0, windowFlags);
	ImGuizmo::SetDrawlist();
	float windowWidth = (float)ImGui::GetWindowWidth();
	float windowHeight = (float)ImGui::GetWindowHeight();
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	viewportInnerRect = window->InnerRect;
	hoveringOnViewport = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max);
	viewportFocused = ImGui::IsWindowFocused();
	windowFlags = hoveringOnViewport ? ImGuiWindowFlags_NoMove : 0;

	auto texture = gameFramebuffer->getDesc().colorAttachments[0].texture;
	viewportDimensions.x = viewportInnerRect.GetHeight() / EngineState::GetViewportHeight() * EngineState::GetViewportWidth();
	viewportDimensions.y = viewportInnerRect.GetHeight();
	ImGui::Image(texture, viewportDimensions, ImVec2{0, 0}, ImVec2{1, 1});

	if (!game_->GetSelectedUuids().empty())
	{
		DrawGizmo();
	}

	ImGui::End();
	ImGui::PopStyleColor(1);
}

void D3E::Editor::DrawHeader()
{
}

void D3E::Editor::DrawPlay()
{
	ImGuiWindowClass window_class;
	window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResizeY | ImGuiDockNodeFlags_NoUndocking;
	ImGui::SetNextWindowClass(&window_class);
	static ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar |
	                                      ImGuiWindowFlags_NoScrollWithMouse |
	                                      ImGuiWindowFlags_NoTitleBar;
	ImGui::Begin("Play", 0, windowFlags);
	ImGuiStyle& style = ImGui::GetStyle();
	float width = 0.0f;
	AlignForWidth(96.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::ImageButtonEx(ImGui::GetID("PlayButton"), TextureFactory::GetTextureHandle("a4e0c3f6-8615-4504-bbb6-16ab19891f3d"), {32.0f, 32.0f}, {0, 0}, {1, 1}, ImVec4(0.3f, 0.3f, 0.3f, game_->IsGameRunning()), ImVec4(1, 1, 1, 1));
	if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		game_->OnEditorPlayPressed();
	}
	ImGui::PopStyleColor();
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::ImageButtonEx(ImGui::GetID("PauseButton"), TextureFactory::GetTextureHandle("302abece-d4bf-4875-bc39-8c32bd15f1ef"), {32.0f, 32.0f}, {0, 0}, {1, 1}, ImVec4(0.3f, 0.3f, 0.3f, game_->IsGamePaused()), ImVec4(1, 1, 1, 1));
	if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		game_->OnEditorPausePressed();
	}
	ImGui::PopStyleColor();
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::ImageButton(TextureFactory::GetTextureHandle("30e4f8eb-08e0-4633-b9f7-207ec70db06e"), {32.0f, 32.0f}, {0, 0}, {1, 1});
	if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		game_->OnEditorStopPressed();
	}
	ImGui::PopStyleColor();
	ImGui::End();
}

void D3E::Editor::AlignForWidth(float width, float alignment)
{
	ImGuiStyle& style = ImGui::GetStyle();
	float avail = ImGui::GetContentRegionAvail().x;
	float off = (avail - width) * alignment;
	if (off > 0.0f)
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
}

void D3E::Editor::DrawHierarchy()
{
	ImGui::Begin("Hierarchy");
	auto objects = EditorUtils::ListActiveObjects();

	eastl::map<String, HierarchiNode> tree;

	tree.insert(EmptyIdString);

	for (auto & object : objects)
	{
		tree.insert({object.infoComponent->id, {object, {}}});
	}

	for (auto& node : tree)
	{
		if (node.first == EmptyIdString)
		{
			continue;
		}

		if (tree.find(node.second.info.infoComponent->parentId) == tree.end())
		{
			Debug::LogError("[Editor] Object has invalid parent ID!");
			continue;
		}

		tree[node.second.info.infoComponent->parentId].children.push_back(&node.second);
	}

	String uuidClicked = "";
	for (auto node : tree[EmptyIdString].children)
	{
		ImGuiTreeNodeFlags node_flags = (node->info.selected ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)(intptr_t)(node->info.infoComponent->editorId), node_flags, "%s", node->info.infoComponent->name.c_str());
		if (ImGui::IsItemClicked())
		{
			uuidClicked = node->info.infoComponent->id;
		}
		if (opened)
		{
			for (auto childNode : node->children)
			{
				DrawHierarchyNode(childNode, uuidClicked);
			}
			ImGui::TreePop();
		}
	}

	if (!uuidClicked.empty())
	{
		if (ImGui::GetIO().KeyCtrl)
		{
			game_->SetUuidEditorSelected(uuidClicked, !game_->IsUuidEditorSelected(uuidClicked), false);
		}
		else
		{
			game_->SetUuidEditorSelected(uuidClicked, true, true);
		}
	}

	ImGui::End();
}
void D3E::Editor::DrawInspector()
{
	ImGui::Begin("Inspector");
	ImGui::Text("Here will be some useful information about object");
	ImGui::End();
}

void D3E::Editor::DrawGizmo()
{
	Vector3 origin = {0, 0, 0};

	auto playerView =
		game_->GetRegistry()
			.view<const TransformComponent, const CameraComponent>();

	const CameraComponent* camera = nullptr;

	for (auto [entity, tc, cc] : playerView.each())
	{
		origin = tc.position + cc.offset;
		camera = &cc;
		break;
	}

	if (!camera)
	{
		Debug::LogWarning("[Editor] Camera not found");
		return;
	}

	float windowWidth = viewportDimensions.x; //(float)ImGui::GetWindowWidth();
	float windowHeight = viewportDimensions.y; //(float)ImGui::GetWindowHeight();
	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
	float viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
	float viewManipulateTop = ImGui::GetWindowPos().y;

	usingGizmo = ImGuizmo::IsUsing();

	ImGuizmo::Manipulate((float*)CameraUtils::GetView(origin, *camera).m, (float*)CameraUtils::GetProj(*camera).m, mCurrentGizmoOperation, mCurrentGizmoMode, (float*)game_->GetGizmoTransform().m, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

	// Cube view manipulation, idk if needed - need extracting camera vectors from view matrix to work
	//ImGuizmo::ViewManipulate((float*)CameraUtils::GetView(origin, *camera).m, camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);

	auto view = game_->GetRegistry().view<const ObjectInfoComponent, TransformComponent>();

	view.each([&](const auto& info, auto& tc)
		  {
				  if (!game_->IsUuidEditorSelected(info.id))
				  {
					  return;
				  }
				  Matrix m = game_->GetGizmoTransform() * game_->GetGizmoOffset(info.id);
				  Vector3 pos, scale;
				  Quaternion rot;
				  m.Decompose(scale, rot, pos);
				  tc.position = pos;
				  tc.rotation = rot;
				  tc.scale = scale;
			  });

	game_->CalculateGizmoTransformsOffsets();
}

void D3E::Editor::DrawHierarchyNode(D3E::Editor::HierarchiNode* node,
                                    D3E::String& uuidClicked)
{
	ImGuiTreeNodeFlags node_flags = (node->info.selected ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
	bool opened = ImGui::TreeNodeEx((void*)(intptr_t)(node->info.infoComponent->editorId), node_flags, "%s", node->info.infoComponent->name.c_str());
	if (ImGui::IsItemClicked())
	{
		uuidClicked = node->info.infoComponent->id;
	}
	if (opened)
	{
		for (auto childNode : node->children)
		{
			DrawHierarchyNode(childNode, uuidClicked);
		}
		ImGui::TreePop();
	}
}

bool D3E::Editor::IsMouseOnViewport()
{
	return hoveringOnViewport;
}

void D3E::Editor::GetMousePositionInViewport(int& mouseX, int& mouseY)
{
	Vector2 globalMousePos = game_->GetInputDevice()->MousePosition;
	mouseX = static_cast<int>((globalMousePos.x - viewportInnerRect.Min.x) / viewportDimensions.x * EngineState::GetViewportWidth());
	mouseY = static_cast<int>((globalMousePos.y - viewportInnerRect.Min.y) / viewportDimensions.y * EngineState::GetViewportHeight());
}

void D3E::Editor::DrawTransformEdit()
{
	ImGui::Begin("Transform Edit");

	if (ImGui::IsKeyPressed(ImGuiKey_T))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(ImGuiKey_E))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	if (ImGui::RadioButton("Universal", mCurrentGizmoOperation == ImGuizmo::UNIVERSAL))
		mCurrentGizmoOperation = ImGuizmo::UNIVERSAL;
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents((float*)game_->GetGizmoTransform().m, matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Tr", matrixTranslation);
	ImGui::InputFloat3("Rt", matrixRotation);
	ImGui::InputFloat3("Sc", matrixScale);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, (float*)game_->GetGizmoTransform().m);

	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
			mCurrentGizmoMode = ImGuizmo::WORLD;
	}
	if (ImGui::IsKeyPressed(ImGuiKey_S))
		useSnap = !useSnap;
	ImGui::Checkbox("##UseSnap", &useSnap);
	ImGui::SameLine();

	switch (mCurrentGizmoOperation)
	{
		case ImGuizmo::TRANSLATE:
			ImGui::InputFloat3("Snap", &snap[0]);
			break;
		case ImGuizmo::ROTATE:
			ImGui::InputFloat("Angle Snap", &snap[0]);
			break;
		case ImGuizmo::SCALE:
			ImGui::InputFloat("Scale Snap", &snap[0]);
			break;
	}
	ImGui::Checkbox("Bound Sizing", &boundSizing);
	if (boundSizing)
	{
		ImGui::PushID(3);
		ImGui::Checkbox("##BoundSizing", &boundSizingSnap);
		ImGui::SameLine();
		ImGui::InputFloat3("Snap", boundsSnap);
		ImGui::PopID();
	}

	ImGui::End();
}
