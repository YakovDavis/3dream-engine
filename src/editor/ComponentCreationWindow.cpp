#include "ComponentCreationWindow.h"

#include "D3E/Game.h"
#include "Editor.h"
#include "misc/cpp/imgui_stdlib.h"
#include "input/InputDevice.h"

D3E::ComponentCreationWindow::ComponentCreationWindow(D3E::Game* game, D3E::Editor* editor)
{
	game_ = game;
	editor_ = editor;
	open = false;
}

void D3E::ComponentCreationWindow::Draw()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse;

	ImGui::SetNextWindowSize(ImVec2(400, 400));

	switch (componentType)
	{
		case 0:
			ImGui::Begin("Create FPSControllerComponent", nullptr, flags);
			if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && editor_->lmbDownLastFrame)
			{
				ImGui::SetWindowFocus();
			}

			float yaw, pitch, speed, sensitivityX, sensitivityY;
			ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
			std::string yawInput = std::to_string(fpsControllerComponent.yaw);
			if (ImGui::InputText("Yaw", &yawInput, input_text_flags))
			{
				if (!(yawInput.empty()))
				{
					yaw = std::stof(yawInput);
					fpsControllerComponent.yaw = yaw;
				}
			}
			std::string pitchInput = std::to_string(fpsControllerComponent.pitch);
			if (ImGui::InputText("Pitch", &pitchInput, input_text_flags))
			{
				if (!(pitchInput.empty()))
				{
					pitch = std::stof(pitchInput);
					fpsControllerComponent.pitch = pitch;
				}
			}
			std::string speedInput = std::to_string(fpsControllerComponent.speed);
			if (ImGui::InputText("Speed", &speedInput, input_text_flags))
			{
				if (!(speedInput.empty()))
				{
					speed = std::stof(speedInput);
					if (speed > 0.0f)
					{
						fpsControllerComponent.speed = speed;
					}
				}
			}
			std::string sensitivityXInput = std::to_string(fpsControllerComponent.sensitivityX);
			if (ImGui::InputText("Sensitivity X", &sensitivityXInput, input_text_flags))
			{
				if (!(sensitivityXInput.empty()))
				{
					sensitivityX = std::stof(sensitivityXInput);
					if (sensitivityX > 0.0f)
					{
						fpsControllerComponent.sensitivityX = sensitivityX;
					}
				}
			}
			std::string sensitivityYInput = std::to_string(fpsControllerComponent.sensitivityY);
			if (ImGui::InputText("Sensitivity Y", &sensitivityYInput, input_text_flags))
			{
				if (!(sensitivityYInput.empty()))
				{
					sensitivityY = std::stof(sensitivityYInput);
					if (sensitivityY > 0.0f)
					{
						fpsControllerComponent.sensitivityY = sensitivityY;
					}
				}
			}
			int isLMBActivated = fpsControllerComponent.isLMBActivated;
			int isRMBActivated = fpsControllerComponent.isRMBActivated;
			ImGui::RadioButton("Is LMB Activated Off", &isLMBActivated, 0);
			ImGui::RadioButton("Is LMB Activated On", &isLMBActivated, 1);
			fpsControllerComponent.isLMBActivated = isLMBActivated;

			ImGui::RadioButton("Is RMB Activated Off", &isRMBActivated, 0);
			ImGui::RadioButton("Is RMB Activated On", &isRMBActivated, 1);
			fpsControllerComponent.isRMBActivated = isRMBActivated;

			ImGui::Separator();
			ImGui::Button("Create", ImVec2(0, 0));
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				game_->GetRegistry().emplace<FPSControllerComponent>(currentEntity, fpsControllerComponent);
				fpsControllerComponent = {};
				open = false;
			}
			ImGui::SameLine();
			ImGui::Button("Cancel", ImVec2(0, 0));
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				fpsControllerComponent = {};
				open = false;
			}
			ImGui::End();
			break;
	}
}
