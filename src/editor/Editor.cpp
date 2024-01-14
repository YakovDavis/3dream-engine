#include "Editor.h"

#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Components/TransformComponent.h"
#include "D3E/Components/FPSControllerComponent.h"
#include "D3E/Components/PhysicsComponent.h"
#include "D3E/Components/PhysicsCharacterComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Components/render/LightComponent.h"
#include "D3E/Components/render/StaticMeshComponent.h"
#include "D3E/Components/sound/SoundComponent.h"
#include "D3E/Components/ScriptComponent.h"
#include "D3E/Debug.h"
#include "D3E/Game.h"
#include "D3E/systems/CreationSystems.h"
#include "ImGuizmo.h"
#include "assetmng/TextureFactory.h"
#include "core/EngineState.h"
#include "imgui_internal.h"
#include "input/InputDevice.h"
#include "nvrhi/nvrhi.h"
#include "render/CameraUtils.h"
#include "render/DisplayWin32.h"
#include "engine/ComponentFactory.h"
#include "assetmng/ScriptFactory.h"
#include "misc/cpp/imgui_stdlib.h"
#include "SimpleMath.h"

#include "D3E/AssetManager.h"
#include <assetmng/MeshFactory.h>
#include <assetmng/MaterialFactory.h>
#include "misc/cpp/imgui_stdlib.h"

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

#define HIERARCHY_DRAG_N_DROP \
	if (ImGui::BeginDragDropSource()) \
	{ \
		std::string entityUuid = node->info.infoComponent->id.c_str(); \
		ImGui::SetDragDropPayload("hierarchy", &entityUuid[0], entityUuid.size() + sizeof(std::string::value_type)); \
		ImGui::EndDragDropSource(); \
	} \
	if (ImGui::BeginDragDropTarget()) \
	{ \
		auto payload = ImGui::AcceptDragDropPayload("hierarchy"); \
		if (payload) \
		{ \
			auto payloadId = (const char*)payload->Data; \
			entt::entity child; \
			game_->FindEntityByID(child, payloadId); \
			auto info = game_->GetRegistry().try_get<ObjectInfoComponent>(child); \
			if (info) \
			{ \
				info->parentId = node->info.infoComponent->id; \
			} \
		} \
		ImGui::EndDragDropTarget(); \
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
	materialEditor_ = new MaterialEditor(this);
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
	imGuiNvrhi_.beginFrame(deltaTime / 1000.0f, displaySize);
}

void D3E::Editor::EndDraw(nvrhi::IFramebuffer* currentFramebuffer, nvrhi::IFramebuffer* gameFramebuffer)
{
	bool show;
	ShowEditorApp(&show);

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
	if (materialEditor_->open)
	{
		materialEditor_->Draw();
	}

	if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
	{
		editorContentBrowser_->ResetTempUuid();
	}

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

	lmbDownLastFrame = ImGui::IsMouseDown(ImGuiMouseButton_Left);

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

static D3E::String hierarchyCarriedUuid = "";
static D3E::String hierarchyRenamedItemUuid = "";
static std::string hierarchyRenamedString = "";

void D3E::Editor::DrawHierarchy()
{
	ImGui::Begin("Hierarchy");

	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && lmbDownLastFrame)
	{
		ImGui::SetWindowFocus();
	}

	// Load prefab dragged on hierarchy
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && lmbDownLastFrame && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
	{
		if (!editorContentBrowser_->GetTempUuid().empty())
		{
			if (AssetManager::IsPrefabUuidValid(editorContentBrowser_->GetTempUuid().c_str()))
			{
				game_->CreateEntityFromPrefab(AssetManager::GetPrefabFilePath(editorContentBrowser_->GetTempUuid().c_str()));
			}
		}
	}

	static int createItem = 0;
	ImGui::Combo("##create_combo", &createItem, "Empty\0Plane\0Cube\0Sphere\0Light\0\0");
	ImGui::SameLine();
	ImGui::Button("Create", ImVec2(0, 0));
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		CreationSystems::OnCreateObjectButtonPressed(game_->GetRegistry(), createItem);
	}

	ImGui::Separator();

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
		ImGuiTreeNodeFlags node_flags = (node->info.selected ? ImGuiTreeNodeFlags_Selected : 0) |
		                                ImGuiTreeNodeFlags_OpenOnArrow |
		                                (node->children.empty() ? ImGuiTreeNodeFlags_Leaf : 0);
		bool opened = false;
		ImGui::PushID(node->info.infoComponent->editorId);
		if (node->info.infoComponent->id == hierarchyRenamedItemUuid)
		{
			ImGui::InputText("##renamed_hierarchy", &hierarchyRenamedString);
			if (ImGui::IsKeyDown(ImGuiKey_Enter))
			{
				node->info.infoComponent->name = hierarchyRenamedString.c_str();
				hierarchyRenamedItemUuid = "";
			}
		}
		else
		{
			opened = ImGui::TreeNodeEx(
				(void*)(intptr_t)(node->info.infoComponent->editorId),
				node_flags, "%s", node->info.infoComponent->name.c_str());
		}
		HIERARCHY_DRAG_N_DROP
		if (ImGui::IsItemClicked())
		{
			if (ImGui::IsKeyDown(ImGuiKey_F2))
			{
				hierarchyRenamedItemUuid = node->info.infoComponent->id;
				hierarchyRenamedString = node->info.infoComponent->name.c_str();
			}
			else
			{
				uuidClicked = node->info.infoComponent->id;
				hierarchyCarriedUuid = uuidClicked;
			}
		}
		ImGui::PopID();
		if (opened)
		{
			for (auto childNode : node->children)
			{
				DrawHierarchyNode(childNode, uuidClicked);
			}
			ImGui::TreePop();
		}
	}

	ImGui::Separator();

	ImGui::PushID(-1);
	ImGui::Text("---Drag here to un-parent---");
	if (ImGui::BeginDragDropTarget()) \
	{ \
		auto payload = ImGui::AcceptDragDropPayload("hierarchy"); \
		if (payload) \
		{ \
			auto payloadId = (const char*)payload->Data; \
			entt::entity child; \
			game_->FindEntityByID(child, payloadId); \
			auto info = game_->GetRegistry().try_get<ObjectInfoComponent>(child); \
			if (info) \
			{ \
				info->parentId = EmptyIdString; \
			} \
		} \
		ImGui::EndDragDropTarget(); \
	}
	ImGui::PopID();

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

	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && lmbDownLastFrame)
	{
		ImGui::SetWindowFocus();
	}

	const eastl::hash_set<String>& objectUuids(game_->GetSelectedUuids());
	if (objectUuids.size() == 1)
	{
		String currentUuid = *objectUuids.begin();
		entt::entity currentEntity = entt::null;
		if (game_->FindEntityByID(currentEntity, currentUuid))
		{
			eastl::vector<D3E::String> currentComponents(ComponentFactory::GetAllEntityComponents(currentEntity));
			auto& infoComponent = game_->GetRegistry().get<ObjectInfoComponent>(currentEntity);
			ImGui::Text(infoComponent.name.c_str());
			size_t idx = 0;
			for (auto componentName : currentComponents)
			{
				ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow;
				bool opened = ImGui::TreeNodeEx((void*)(intptr_t)(idx), node_flags, "%s", componentName.c_str());
				if (opened)
				{
					if (componentName == "TransformComponent")
					{
						//game_->GetRegistry().patch<TransformComponent>(currentEntity, [](auto& transform){ transform.position = {10, 10, 10, 1.0f};});
						size_t fieldIdx = idx * 100;
						bool positionOpened = ImGui::TreeNodeEx((void*)(intptr_t)(fieldIdx), node_flags, "%s", "Position");
						if (positionOpened)
						{
							float x, y, z;
							ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
							std::string xInput = std::to_string(game_->GetRegistry().get<TransformComponent>(currentEntity).position.x);
							if (ImGui::InputText("x", &xInput, input_text_flags))
							{
								if (!(xInput.empty()))
								{
									x = std::stof(xInput);
									game_->GetRegistry().patch<TransformComponent>(currentEntity, [x](auto &transform) { transform.position.x = x; std::cout << x << "\n"; });
								}
							}
							std::string yInput = std::to_string(game_->GetRegistry().get<TransformComponent>(currentEntity).position.y);
							if (ImGui::InputText("y", &yInput, input_text_flags))
							{
								if (!(yInput.empty()))
								{
									y = std::stof(yInput);
									game_->GetRegistry().patch<TransformComponent>(currentEntity, [y](auto &transform) { transform.position.y = y; std::cout << y << "\n"; });
								}
							}
							std::string zInput = std::to_string(game_->GetRegistry().get<TransformComponent>(currentEntity).position.z);
							if (ImGui::InputText("z", &zInput, input_text_flags))
							{
								if (!(zInput.empty()))
								{
									z = std::stof(zInput);
									game_->GetRegistry().patch<TransformComponent>(currentEntity, [z](auto &transform) { transform.position.z = z; std::cout << z << "\n";});
								}
							}
							ImGui::TreePop();
						}
						++fieldIdx;
						bool rotationOpened = ImGui::TreeNodeEx((void*)(intptr_t)(fieldIdx), node_flags, "%s", "Rotation");
						if (rotationOpened)
						{
							float x, y, z;
							ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
							std::string xInput = std::to_string(game_->GetRegistry().get<TransformComponent>(currentEntity).rotation.x);
							if (ImGui::InputText("x", &xInput, input_text_flags))
							{
								if (!(xInput.empty()))
								{
									x = std::stof(xInput);
									game_->GetRegistry().patch<TransformComponent>(currentEntity, [x](auto &transform) { transform.rotation.x = x; });
								}
							}
							std::string yInput = std::to_string(game_->GetRegistry().get<TransformComponent>(currentEntity).rotation.y);
							if (ImGui::InputText("y", &yInput, input_text_flags))
							{
								if (!(yInput.empty()))
								{
									y = std::stof(yInput);
									game_->GetRegistry().patch<TransformComponent>(currentEntity, [y](auto &transform) { transform.rotation.y = y; });
								}
							}
							std::string zInput = std::to_string(game_->GetRegistry().get<TransformComponent>(currentEntity).rotation.z);
							if (ImGui::InputText("z", &zInput, input_text_flags))
							{
								if (!(zInput.empty()))
								{
									z = std::stof(zInput);
									game_->GetRegistry().patch<TransformComponent>(currentEntity, [z](auto &transform) { transform.rotation.z = z; });
								}
							}
							ImGui::TreePop();
						}
						++fieldIdx;
						bool scaleOpened = ImGui::TreeNodeEx((void*)(intptr_t)(fieldIdx), node_flags, "%s", "Scale");
						if (scaleOpened)
						{
							float x, y, z;
							ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
							std::string xInput = std::to_string(game_->GetRegistry().get<TransformComponent>(currentEntity).scale.x);
							if (ImGui::InputText("x", &xInput, input_text_flags))
							{
								if (!(xInput.empty()))
								{
									x = std::stof(xInput);
									if (x > 0.0f)
									{
										game_->GetRegistry().patch<TransformComponent>(currentEntity, [x](auto &transform) { transform.scale.x = x; });
									}
								}
							}
							std::string yInput = std::to_string(game_->GetRegistry().get<TransformComponent>(currentEntity).scale.y);
							if (ImGui::InputText("y", &yInput, input_text_flags))
							{
								if (!(yInput.empty()))
								{
									y = std::stof(yInput);
									if (y > 0.0f)
									{
										game_->GetRegistry().patch<TransformComponent>(currentEntity, [y](auto &transform) { transform.scale.y = y; });
									}
								}
							}
							std::string zInput = std::to_string(game_->GetRegistry().get<TransformComponent>(currentEntity).scale.z);
							if (ImGui::InputText("z", &zInput, input_text_flags))
							{
								if (!(zInput.empty()))
								{
									z = std::stof(zInput);
									if (z > 0.0f)
									{
										game_->GetRegistry().patch<TransformComponent>(currentEntity, [z](auto &transform) { transform.scale.z = z; });
									}
								}
							}
							ImGui::TreePop();
						}
					}
					else if (componentName == "FPSControllerComponent")
					{
						size_t fieldIdx = idx * 100;
						float yaw, pitch, speed;
						ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
						std::string yawInput = std::to_string(game_->GetRegistry().get<FPSControllerComponent>(currentEntity).yaw);
						if (ImGui::InputText("Yaw", &yawInput, input_text_flags))
						{
							if (!(yawInput.empty()))
							{
								yaw = std::stof(yawInput);
								yaw +=
									game_->GetRegistry().get<FPSControllerComponent>(currentEntity).sensitivityX * game_->GetInputDevice()->MouseOffsetInTick.x;
								while (yaw < -DirectX::XM_2PI)
									yaw += DirectX::XM_2PI;
								while (yaw > DirectX::XM_2PI)
									yaw -= DirectX::XM_2PI;
								game_->GetRegistry().patch<FPSControllerComponent>(currentEntity, [yaw](auto &controller) { controller.yaw = yaw; });
							}
						}
						std::string pitchInput = std::to_string(game_->GetRegistry().get<FPSControllerComponent>(currentEntity).pitch);
						if (ImGui::InputText("Pitch", &pitchInput, input_text_flags))
						{
							if (!(pitchInput.empty()))
							{
								pitch = std::stof(pitchInput);
								pitch -=
									game_->GetRegistry().get<FPSControllerComponent>(currentEntity).sensitivityY * game_->GetInputDevice()->MouseOffsetInTick.y;
								while (pitch < -DirectX::XM_2PI)
									pitch += DirectX::XM_2PI;
								while (pitch > DirectX::XM_2PI)
									pitch -= DirectX::XM_2PI;
								game_->GetRegistry().patch<FPSControllerComponent>(currentEntity, [pitch](auto &controller) { controller.pitch = pitch; });
							}
						}
						std::string speedInput = std::to_string(game_->GetRegistry().get<FPSControllerComponent>(currentEntity).speed);
						if (ImGui::InputText("Speed", &speedInput, input_text_flags))
						{
							if (!(speedInput.empty()))
							{
								speed = std::stof(speedInput);
								if (speed > 0.0f)
								{
									game_->GetRegistry().patch<FPSControllerComponent>(currentEntity, [speed](auto &controller) { controller.speed = speed; });
								}
							}
						}
					}
					else if (componentName == "PhysicsComponent")
					{
						float friction, restitution;
						ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
						std::string frictionInput = std::to_string(game_->GetRegistry().get<PhysicsComponent>(currentEntity).friction_);
						if (ImGui::InputText("Friction", &frictionInput, input_text_flags))
						{
							if (!(frictionInput.empty()))
							{
								friction = std::stof(frictionInput);
								if (friction >= 0.0f)
								{
									game_->GetRegistry().patch<PhysicsComponent>(currentEntity, [friction](auto &component) { component.friction_ = friction; });
								}
							}
						}
						std::string restitutionInput = std::to_string(game_->GetRegistry().get<PhysicsComponent>(currentEntity).restitution_);
						if (ImGui::InputText("Restitution", &restitutionInput, input_text_flags))
						{
							if (!(restitutionInput.empty()))
							{
								restitution = std::stof(restitutionInput);
								if (restitution >= 0.0f)
								{
									game_->GetRegistry().patch<PhysicsComponent>(currentEntity, [restitution](auto &component) { component.restitution_ = restitution; });
								}
							}
						}
						size_t fieldIdx = idx * 100;
						bool velocityOpened = ImGui::TreeNodeEx((void*)(intptr_t)(fieldIdx), node_flags, "%s", "Velocity");
						if (velocityOpened)
						{
							float x, y, z;
							std::string xInput = std::to_string(game_->GetRegistry().get<PhysicsComponent>(currentEntity).velocity_.x);
							if (ImGui::InputText("x", &xInput, input_text_flags))
							{
								if (!(xInput.empty()))
								{
									x = std::stof(xInput);
									game_->GetRegistry().patch<PhysicsComponent>(currentEntity, [x](auto &component) { component.velocity_.x = x; });
								}
							}
							std::string yInput = std::to_string(game_->GetRegistry().get<PhysicsComponent>(currentEntity).velocity_.y);
							if (ImGui::InputText("y", &yInput, input_text_flags))
							{
								if (!(yInput.empty()))
								{
									y = std::stof(yInput);
									game_->GetRegistry().patch<PhysicsComponent>(currentEntity, [y](auto &component) { component.velocity_.y = y; });
								}
							}
							std::string zInput = std::to_string(game_->GetRegistry().get<PhysicsComponent>(currentEntity).velocity_.z);
							if (ImGui::InputText("z", &zInput, input_text_flags))
							{
								if (!(zInput.empty()))
								{
									z = std::stof(zInput);
									game_->GetRegistry().patch<PhysicsComponent>(currentEntity, [z](auto &component) { component.velocity_.z = z; });
								}
							}
							ImGui::TreePop();
						}
						++fieldIdx;
						bool angularVelocityOpened = ImGui::TreeNodeEx((void*)(intptr_t)(fieldIdx), node_flags, "%s", "Angular Velocity");
						if (angularVelocityOpened)
						{
							float x, y, z;
							std::string xInput = std::to_string(game_->GetRegistry().get<PhysicsComponent>(currentEntity).angularVelocity_.x);
							if (ImGui::InputText("x", &xInput, input_text_flags))
							{
								if (!(xInput.empty()))
								{
									x = std::stof(xInput);
									game_->GetRegistry().patch<PhysicsComponent>(currentEntity, [x](auto &component) { component.angularVelocity_.x = x; });
								}
							}
							std::string yInput = std::to_string(game_->GetRegistry().get<PhysicsComponent>(currentEntity).angularVelocity_.y);
							if (ImGui::InputText("y", &yInput, input_text_flags))
							{
								if (!(yInput.empty()))
								{
									y = std::stof(yInput);
									game_->GetRegistry().patch<PhysicsComponent>(currentEntity, [y](auto &component) { component.angularVelocity_.y = y; });
								}
							}
							std::string zInput = std::to_string(game_->GetRegistry().get<PhysicsComponent>(currentEntity).angularVelocity_.z);
							if (ImGui::InputText("z", &zInput, input_text_flags))
							{
								if (!(zInput.empty()))
								{
									z = std::stof(zInput);
									game_->GetRegistry().patch<PhysicsComponent>(currentEntity, [z](auto &component) { component.angularVelocity_.z = z; });
								}
							}
							ImGui::TreePop();
						}
					}
					else if (componentName == "PhysicsCharacterComponent")
					{
						float yaw, pitch, speed, jumpSpeed, friction, restitution;
						ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
						std::string yawInput = std::to_string(game_->GetRegistry().get<PhysicsCharacterComponent>(currentEntity).yaw_);
						if (ImGui::InputText("Yaw", &yawInput, input_text_flags))
						{
							if (!(yawInput.empty()))
							{
								yaw = std::stof(yawInput);
								yaw +=
									game_->GetRegistry().get<PhysicsCharacterComponent>(currentEntity).sensitivityX_ * game_->GetInputDevice()->MouseOffsetInTick.x;
								while (yaw < -DirectX::XM_2PI)
									yaw += DirectX::XM_2PI;
								while (yaw > DirectX::XM_2PI)
									yaw -= DirectX::XM_2PI;
								game_->GetRegistry().patch<PhysicsCharacterComponent>(currentEntity, [yaw](auto &controller) { controller.yaw_ = yaw; });
							}
						}
						std::string pitchInput = std::to_string(game_->GetRegistry().get<PhysicsCharacterComponent>(currentEntity).pitch_);
						if (ImGui::InputText("Pitch", &pitchInput, input_text_flags))
						{
							if (!(pitchInput.empty()))
							{
								pitch = std::stof(pitchInput);
								pitch -=
									game_->GetRegistry().get<PhysicsCharacterComponent>(currentEntity).sensitivityY_ * game_->GetInputDevice()->MouseOffsetInTick.y;
								while (pitch < -DirectX::XM_2PI)
									pitch += DirectX::XM_2PI;
								while (pitch > DirectX::XM_2PI)
									pitch -= DirectX::XM_2PI;
								game_->GetRegistry().patch<PhysicsCharacterComponent>(currentEntity, [pitch](auto &controller) { controller.pitch_ = pitch; });
							}
						}
						std::string speedInput = std::to_string(game_->GetRegistry().get<PhysicsCharacterComponent>(currentEntity).speed_);
						if (ImGui::InputText("Speed", &speedInput, input_text_flags))
						{
							if (!(speedInput.empty()))
							{
								speed = std::stof(speedInput);
								if (speed > 0.0f)
								{
									game_->GetRegistry().patch<PhysicsCharacterComponent>(currentEntity, [speed](auto &controller) { controller.speed_ = speed; });
								}
							}
						}
						std::string jumpSpeedInput = std::to_string(game_->GetRegistry().get<PhysicsCharacterComponent>(currentEntity).jumpSpeed_);
						if (ImGui::InputText("Jump Speed", &jumpSpeedInput, input_text_flags))
						{
							if (!(jumpSpeedInput.empty()))
							{
								jumpSpeed = std::stof(jumpSpeedInput);
								if (jumpSpeed > 0.0f)
								{
									game_->GetRegistry().patch<PhysicsCharacterComponent>(currentEntity, [jumpSpeed](auto &controller) { controller.jumpSpeed_ = jumpSpeed; });
								}
							}
						}
						std::string frictionInput = std::to_string(game_->GetRegistry().get<PhysicsCharacterComponent>(currentEntity).friction_);
						if (ImGui::InputText("Friction", &frictionInput, input_text_flags))
						{
							if (!(frictionInput.empty()))
							{
								friction = std::stof(frictionInput);
								if (friction >= 0.0f)
								{
									game_->GetRegistry().patch<PhysicsCharacterComponent>(currentEntity, [friction](auto &component) { component.friction_ = friction; });
								}
							}
						}
						std::string restitutionInput = std::to_string(game_->GetRegistry().get<PhysicsCharacterComponent>(currentEntity).restitution_);
						if (ImGui::InputText("Restitution", &restitutionInput, input_text_flags))
						{
							if (!(restitutionInput.empty()))
							{
								restitution = std::stof(restitutionInput);
								if (restitution >= 0.0f)
								{
									game_->GetRegistry().patch<PhysicsCharacterComponent>(currentEntity, [restitution](auto &component) { component.restitution_ = restitution; });
								}
							}
						}
						size_t fieldIdx = idx * 100;
						bool velocityOpened = ImGui::TreeNodeEx((void*)(intptr_t)(fieldIdx), node_flags, "%s", "Velocity");
						if (velocityOpened)
						{
							float x, y, z;
							std::string xInput = std::to_string(game_->GetRegistry().get<PhysicsCharacterComponent>(currentEntity).velocity_.x);
							if (ImGui::InputText("x", &xInput, input_text_flags))
							{
								if (!(xInput.empty()))
								{
									x = std::stof(xInput);
									game_->GetRegistry().patch<PhysicsCharacterComponent>(currentEntity, [x](auto &component) { component.velocity_.x = x; });
								}
							}
							std::string yInput = std::to_string(game_->GetRegistry().get<PhysicsCharacterComponent>(currentEntity).velocity_.y);
							if (ImGui::InputText("y", &yInput, input_text_flags))
							{
								if (!(yInput.empty()))
								{
									y = std::stof(yInput);
									game_->GetRegistry().patch<PhysicsCharacterComponent>(currentEntity, [y](auto &component) { component.velocity_.y = y; });
								}
							}
							std::string zInput = std::to_string(game_->GetRegistry().get<PhysicsCharacterComponent>(currentEntity).velocity_.z);
							if (ImGui::InputText("z", &zInput, input_text_flags))
							{
								if (!(zInput.empty()))
								{
									z = std::stof(zInput);
									game_->GetRegistry().patch<PhysicsCharacterComponent>(currentEntity, [z](auto &component) { component.velocity_.z = z; });
								}
							}
							ImGui::TreePop();
						}
						++fieldIdx;
						bool angularVelocityOpened = ImGui::TreeNodeEx((void*)(intptr_t)(fieldIdx), node_flags, "%s", "Angular Velocity");
						if (angularVelocityOpened)
						{
							float x, y, z;
							std::string xInput = std::to_string(game_->GetRegistry().get<PhysicsCharacterComponent>(currentEntity).angularVelocity_.x);
							if (ImGui::InputText("x", &xInput, input_text_flags))
							{
								if (!(xInput.empty()))
								{
									x = std::stof(xInput);
									game_->GetRegistry().patch<PhysicsCharacterComponent>(currentEntity, [x](auto &component) { component.angularVelocity_.x = x; });
								}
							}
							std::string yInput = std::to_string(game_->GetRegistry().get<PhysicsCharacterComponent>(currentEntity).angularVelocity_.y);
							if (ImGui::InputText("y", &yInput, input_text_flags))
							{
								if (!(yInput.empty()))
								{
									y = std::stof(yInput);
									game_->GetRegistry().patch<PhysicsCharacterComponent>(currentEntity, [y](auto &component) { component.angularVelocity_.y = y; });
								}
							}
							std::string zInput = std::to_string(game_->GetRegistry().get<PhysicsCharacterComponent>(currentEntity).angularVelocity_.z);
							if (ImGui::InputText("z", &zInput, input_text_flags))
							{
								if (!(zInput.empty()))
								{
									z = std::stof(zInput);
									game_->GetRegistry().patch<PhysicsCharacterComponent>(currentEntity, [z](auto &component) { component.angularVelocity_.z = z; });
								}
							}
							ImGui::TreePop();
						}
					}
					else if (componentName == "CameraComponent")
					{
						ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
						size_t fieldIdx = idx * 100;
						bool upOpened = ImGui::TreeNodeEx((void*)(intptr_t)(fieldIdx), node_flags, "%s", "Up");
						if (upOpened)
						{
							float x, y, z;
							std::string xInput = std::to_string(game_->GetRegistry().get<CameraComponent>(currentEntity).up.x);
							if (ImGui::InputText("x", &xInput, input_text_flags))
							{
								if (!(xInput.empty()))
								{
									x = std::stof(xInput);
									game_->GetRegistry().patch<CameraComponent>(currentEntity, [x](auto &component) { component.up.x = x; });
								}
							}
							std::string yInput = std::to_string(game_->GetRegistry().get<CameraComponent>(currentEntity).up.y);
							if (ImGui::InputText("y", &yInput, input_text_flags))
							{
								if (!(yInput.empty()))
								{
									y = std::stof(yInput);
									game_->GetRegistry().patch<CameraComponent>(currentEntity, [y](auto &component) { component.up.y = y; });
								}
							}
							std::string zInput = std::to_string(game_->GetRegistry().get<CameraComponent>(currentEntity).up.z);
							if (ImGui::InputText("z", &zInput, input_text_flags))
							{
								if (!(zInput.empty()))
								{
									z = std::stof(zInput);
									game_->GetRegistry().patch<CameraComponent>(currentEntity, [z](auto &component) { component.up.z = z; });
								}
							}
							ImGui::TreePop();
						}
						++fieldIdx;
						bool forwardOpened = ImGui::TreeNodeEx((void*)(intptr_t)(fieldIdx), node_flags, "%s", "Forward");
						if (forwardOpened)
						{
							float x, y, z;
							std::string xInput = std::to_string(game_->GetRegistry().get<CameraComponent>(currentEntity).forward.x);
							if (ImGui::InputText("x", &xInput, input_text_flags))
							{
								if (!(xInput.empty()))
								{
									x = std::stof(xInput);
									game_->GetRegistry().patch<CameraComponent>(currentEntity, [x](auto &component) { component.forward.x = x; });
								}
							}
							std::string yInput = std::to_string(game_->GetRegistry().get<CameraComponent>(currentEntity).forward.y);
							if (ImGui::InputText("y", &yInput, input_text_flags))
							{
								if (!(yInput.empty()))
								{
									y = std::stof(yInput);
									game_->GetRegistry().patch<CameraComponent>(currentEntity, [y](auto &component) { component.forward.y = y; });
								}
							}
							std::string zInput = std::to_string(game_->GetRegistry().get<CameraComponent>(currentEntity).forward.z);
							if (ImGui::InputText("z", &zInput, input_text_flags))
							{
								if (!(zInput.empty()))
								{
									z = std::stof(zInput);
									game_->GetRegistry().patch<CameraComponent>(currentEntity, [z](auto &component) { component.forward.z = z; });
								}
							}
							ImGui::TreePop();
						}
					}
					else if (componentName == "LightComponent")
					{
						float intensity;
						int castsShadowsSelection;
						ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
						LightType lightType = game_->GetRegistry().get<LightComponent>(currentEntity).lightType;
						const char* const types[] = {"Directional", "Point", "Spot"};
						int selectedIdx = lightType;
						ImGui::ListBox("Light Type", &selectedIdx, types, IM_ARRAYSIZE(types));
						game_->GetRegistry().patch<LightComponent>(currentEntity, [selectedIdx](auto& component) {component.lightType =
									static_cast<LightType>(selectedIdx);});
						size_t fieldIdx = idx * 100;
						bool offsetOpened = ImGui::TreeNodeEx((void*)(intptr_t)(fieldIdx), node_flags, "%s", "Offset");
						if (offsetOpened)
						{
							float x, y, z;
							ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
							std::string xInput = std::to_string(game_->GetRegistry().get<LightComponent>(currentEntity).offset.x);
							if (ImGui::InputText("x", &xInput, input_text_flags))
							{
								if (!(xInput.empty()))
								{
									x = std::stof(xInput);
									game_->GetRegistry().patch<LightComponent>(currentEntity, [x](auto &component) { component.offset.x = x; });
								}
							}
							std::string yInput = std::to_string(game_->GetRegistry().get<LightComponent>(currentEntity).offset.y);
							if (ImGui::InputText("y", &yInput, input_text_flags))
							{
								if (!(yInput.empty()))
								{
									y = std::stof(yInput);
									game_->GetRegistry().patch<LightComponent>(currentEntity, [y](auto &component) { component.offset.y = y; });
								}
							}
							std::string zInput = std::to_string(game_->GetRegistry().get<LightComponent>(currentEntity).offset.z);
							if (ImGui::InputText("z", &zInput, input_text_flags))
							{
								if (!(zInput.empty()))
								{
									z = std::stof(zInput);
									game_->GetRegistry().patch<LightComponent>(currentEntity, [z](auto &component) { component.offset.z = z;} );
								}
							}
							ImGui::TreePop();
						}
						++fieldIdx;
						bool directionOpened = ImGui::TreeNodeEx((void*)(intptr_t)(fieldIdx), node_flags, "%s", "Direction");
						if (directionOpened)
						{
							float x, y, z;
							ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
							std::string xInput = std::to_string(game_->GetRegistry().get<LightComponent>(currentEntity).direction.x);
							if (ImGui::InputText("x", &xInput, input_text_flags))
							{
								if (!(xInput.empty()))
								{
									x = std::stof(xInput);
									game_->GetRegistry().patch<LightComponent>(currentEntity, [x](auto &component) { component.direction.x = x; });
								}
							}
							std::string yInput = std::to_string(game_->GetRegistry().get<LightComponent>(currentEntity).direction.y);
							if (ImGui::InputText("y", &yInput, input_text_flags))
							{
								if (!(yInput.empty()))
								{
									y = std::stof(yInput);
									game_->GetRegistry().patch<LightComponent>(currentEntity, [y](auto &component) { component.direction.y = y; });
								}
							}
							std::string zInput = std::to_string(game_->GetRegistry().get<LightComponent>(currentEntity).direction.z);
							if (ImGui::InputText("z", &zInput, input_text_flags))
							{
								if (!(zInput.empty()))
								{
									z = std::stof(zInput);
									game_->GetRegistry().patch<LightComponent>(currentEntity, [z](auto &component) { component.direction.z = z;} );
								}
							}
							ImGui::TreePop();
						}
						++fieldIdx;
						bool colorOpened = ImGui::TreeNodeEx((void*)(intptr_t)(fieldIdx), node_flags, "%s", "Color");
						if (colorOpened)
						{
							float x, y, z;
							ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
							std::string xInput = std::to_string(game_->GetRegistry().get<LightComponent>(currentEntity).color.x);
							if (ImGui::InputText("x", &xInput, input_text_flags))
							{
								if (!(xInput.empty()))
								{
									x = std::stof(xInput);
									if (x >= 0.0f && x <= 1.0f)
									{
										game_->GetRegistry().patch<LightComponent>(currentEntity, [x](auto &component) { component.color.x = x; });
									}
								}
							}
							std::string yInput = std::to_string(game_->GetRegistry().get<LightComponent>(currentEntity).color.y);
							if (ImGui::InputText("y", &yInput, input_text_flags))
							{
								if (!(yInput.empty()))
								{
									y = std::stof(yInput);
									if (y >= 0.0f && y <= 1.0f)
									{
										game_->GetRegistry().patch<LightComponent>(currentEntity, [y](auto &component) { component.color.y = y; });
									}
								}
							}
							std::string zInput = std::to_string(game_->GetRegistry().get<LightComponent>(currentEntity).color.z);
							if (ImGui::InputText("z", &zInput, input_text_flags))
							{
								if (!(zInput.empty()))
								{
									z = std::stof(zInput);
									if (z >= 0.0f && z <= 1.0f)
									{
										game_->GetRegistry().patch<LightComponent>(currentEntity, [z](auto &component) { component.color.z = z;} );
									}
								}
							}
							ImGui::TreePop();
						}
						std::string intensityInput = std::to_string(game_->GetRegistry().get<LightComponent>(currentEntity).intensity);
						if (ImGui::InputText("Intensity", &intensityInput, input_text_flags))
						{
							if (!(intensityInput.empty()))
							{
								intensity = std::stof(intensityInput);
								if (intensity >= 0.0f)
								{
									game_->GetRegistry().patch<LightComponent>(currentEntity, [intensity](auto &component) { component.intensity = intensity; });
								}
							}
						}
						castsShadowsSelection = game_->GetRegistry().get<LightComponent>(currentEntity).castsShadows;
						ImGui::RadioButton("Casts Shadows Off", &castsShadowsSelection, 0);
						ImGui::RadioButton("Casts Shadows On", &castsShadowsSelection, 1);
						game_->GetRegistry().patch<LightComponent>(currentEntity, [castsShadowsSelection](auto &component) { component.castsShadows = castsShadowsSelection; });
					}
					else if (componentName == "StaticMeshComponent")
					{
						ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_ReadOnly;
						String meshUuid = game_->GetRegistry().get<StaticMeshComponent>(currentEntity).meshUuid;
						std::string meshName;
						if (MeshFactory::IsMeshUuidValid(meshUuid))
						{
							meshName = AssetManager::GetAssetName(meshUuid).c_str();
						}
						ImGui::InputText("Mesh", &meshName, input_text_flags);
						if (ImGui::IsItemHovered() && lmbDownLastFrame && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
						{
							std::string selectedUuid = editorContentBrowser_->GetTempUuid();
							if (!selectedUuid.empty() && MeshFactory::IsMeshUuidValid(selectedUuid.c_str()))
							{
								game_->GetRegistry().patch<StaticMeshComponent>(currentEntity, [selectedUuid](auto &component) { component.meshUuid = selectedUuid.c_str(); });
							}
						}

						String materialUuid = game_->GetRegistry().get<StaticMeshComponent>(currentEntity).materialUuid;
						std::string materialName;
						if (MaterialFactory::IsMaterialUuidValid(materialUuid))
						{
							materialName = MaterialFactory::GetMaterial(materialUuid).name.c_str();
						}
						if (ImGui::InputText("Material", &materialName, input_text_flags))
						{
							if (ImGui::IsItemHovered() && lmbDownLastFrame && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
							{
								std::string selectedUuid = editorContentBrowser_->GetTempUuid();
								if (!selectedUuid.empty() && MaterialFactory::IsMaterialUuidValid(selectedUuid.c_str()))
								{
									game_->GetRegistry().patch<StaticMeshComponent>(currentEntity, [selectedUuid](auto &component) { component.materialUuid = selectedUuid.c_str(); });
								}
							}
						}
					}
					else if (componentName == "SoundComponent")
					{
						int is3D, isLooping, isStreaming;
						float volume;
						is3D = game_->GetRegistry().get<SoundComponent>(currentEntity).is3D;
						ImGui::RadioButton("Is 3D Off", &is3D, 0);
						ImGui::RadioButton("Is 3D On", &is3D, 1);
						game_->GetRegistry().patch<SoundComponent>(currentEntity, [is3D](auto &component) { component.is3D = is3D; });
						isLooping = game_->GetRegistry().get<SoundComponent>(currentEntity).isLooping;
						ImGui::RadioButton("Is Looping Off", &isLooping, 0);
						ImGui::RadioButton("Is Looping On", &isLooping, 1);
						game_->GetRegistry().patch<SoundComponent>(currentEntity, [isLooping](auto &component) { component.isLooping = isLooping; });
						isStreaming = game_->GetRegistry().get<SoundComponent>(currentEntity).isStreaming;
						ImGui::RadioButton("Is Streaming Off", &isStreaming, 0);
						ImGui::RadioButton("Is Streaming On", &isStreaming, 1);
						game_->GetRegistry().patch<SoundComponent>(currentEntity, [isStreaming](auto &component) { component.isStreaming = isStreaming; });
						ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
						std::string volumeInput = std::to_string(game_->GetRegistry().get<SoundComponent>(currentEntity).volume);
						if (ImGui::InputText("Volume", &volumeInput, input_text_flags))
						{
							if (!(volumeInput.empty()))
							{
								volume = std::stof(volumeInput);
								if (volume >= 0.0f)
								{
									game_->GetRegistry().patch<SoundComponent>(currentEntity, [volume](auto &component) { component.volume = volume; });
								}
							}
						}
						size_t fieldIdx = idx * 100;
						bool locationOpened = ImGui::TreeNodeEx((void*)(intptr_t)(fieldIdx), node_flags, "%s", "Location");
						if (locationOpened)
						{
							float x, y, z;
							ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
							std::string xInput = std::to_string(game_->GetRegistry().get<SoundComponent>(currentEntity).location.x);
							if (ImGui::InputText("x", &xInput, input_text_flags))
							{
								if (!(xInput.empty()))
								{
									x = std::stof(xInput);
									game_->GetRegistry().patch<SoundComponent>(currentEntity, [x](auto &component) { component.location.x = x; });
								}
							}
							std::string yInput = std::to_string(game_->GetRegistry().get<SoundComponent>(currentEntity).location.y);
							if (ImGui::InputText("y", &yInput, input_text_flags))
							{
								if (!(yInput.empty()))
								{
									y = std::stof(yInput);
									game_->GetRegistry().patch<SoundComponent>(currentEntity, [y](auto &component) { component.location.y = y; });
								}
							}
							std::string zInput = std::to_string(game_->GetRegistry().get<SoundComponent>(currentEntity).location.z);
							if (ImGui::InputText("z", &zInput, input_text_flags))
							{
								if (!(zInput.empty()))
								{
									z = std::stof(zInput);
									game_->GetRegistry().patch<SoundComponent>(currentEntity, [z](auto &component) { component.location.z = z;} );
								}
							}
							ImGui::TreePop();
						}
					}
					else if (componentName == "ScriptComponent")
					{
						ImGuiInputTextFlags input_text_flags =
							ImGuiInputTextFlags_ReadOnly;

						auto& sc = game_->GetRegistry().get<ScriptComponent>(
							currentEntity);
						const String scriptUuid = sc.GetScriptUuid();
						
						std::string scriptName;
						if (ScriptFactory::IsScriptUuidValid(scriptUuid))
						{
							scriptName =
								AssetManager::GetAssetName(scriptUuid).c_str();
						}
						ImGui::InputText("Script", &scriptName,
						                 input_text_flags);

						if (ImGui::IsItemHovered() && lmbDownLastFrame &&
						    !ImGui::IsMouseDown(ImGuiMouseButton_Left))
						{
							std::string selectedUuid =
								editorContentBrowser_->GetTempUuid();
							if (!selectedUuid.empty() &&
							    ScriptFactory::IsScriptUuidValid(
									selectedUuid.c_str()))
							{
								game_->GetRegistry().patch<ScriptComponent>(
									currentEntity,
									[selectedUuid](auto& sc) {
										sc.SetScriptUuid(selectedUuid.c_str());
									});
							}
						}
					}

					ImGui::TreePop();
				}
				++idx;
			}

		}
		else
		{
			ImGui::Text("Error: Incorrect ID");
		}
	}
	else if (objectUuids.empty())
	{
		ImGui::Text("No object selected");
	}
	else
	{
		ImGui::Text("Multiple objects selected");
	}

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
	ImGuiTreeNodeFlags node_flags = (node->info.selected ? ImGuiTreeNodeFlags_Selected : 0) |
	                                ImGuiTreeNodeFlags_OpenOnArrow |
	                                (node->children.empty() ? ImGuiTreeNodeFlags_Leaf : 0);
	ImGui::PushID(node->info.infoComponent->editorId);
	bool opened = false;
	if (node->info.infoComponent->id == hierarchyRenamedItemUuid)
	{
		ImGui::InputText("##renamed_hierarchy", &hierarchyRenamedString);
		if (ImGui::IsKeyDown(ImGuiKey_Enter))
		{
			node->info.infoComponent->name = hierarchyRenamedString.c_str();
			hierarchyRenamedItemUuid = "";
		}
	}
	else
	{
		opened = ImGui::TreeNodeEx(
			(void*)(intptr_t)(node->info.infoComponent->editorId),
			node_flags, "%s", node->info.infoComponent->name.c_str());
	}
	HIERARCHY_DRAG_N_DROP
	if (ImGui::IsItemClicked())
	{
		if (ImGui::IsKeyDown(ImGuiKey_F2))
		{
			hierarchyRenamedItemUuid = node->info.infoComponent->id;
			hierarchyRenamedString = node->info.infoComponent->name.c_str();
		}
		else
		{
			uuidClicked = node->info.infoComponent->id;
			hierarchyCarriedUuid = uuidClicked;
		}
	}
	if (!ImGui::IsMouseDown(ImGuiMouseButton_Left) && lmbDownLastFrame)
	{
		// TODO: link new parent
	}
	ImGui::PopID();
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

void D3E::Editor::ShowEditorApp(bool* p_open)
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
			if (ImGui::MenuItem("Save map", "Ctrl+S", false, p_open != NULL))
			{
				game_->OnEditorSaveMapPressed();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	if (ImGui::IsKeyPressed(ImGuiKey_LeftCtrl))
	{
		if (ImGui::IsKeyPressed(ImGuiKey_S))
		{
			game_->OnEditorSaveMapPressed();
		}
	}

	ImGui::End();
}
