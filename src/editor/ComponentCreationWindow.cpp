#include "ComponentCreationWindow.h"

#include "D3E/Game.h"
#include "Editor.h"
#include "misc/cpp/imgui_stdlib.h"
#include "input/InputDevice.h"
#include "render/systems/LightInitSystem.h"

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
		{
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
		case 1:
		{
			ImGui::Begin("Create PhysicsComponent", nullptr, flags);
			if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && editor_->lmbDownLastFrame)
			{
				ImGui::SetWindowFocus();
			}

			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow;
			float mass, friction, restitution;
			int hasOffsetCenterOfMassSelection, isSensorSelection;
			ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
			ColliderType colliderType = physicsComponent.colliderType_;
			int selectedIdx = static_cast<int>(colliderType);
			ImGui::Combo("Collider Type", &selectedIdx, "SphereCollider\0BoxCollider\0CapsuleCollider\0TaperedCapsuleCollider\0CylinderCollider\0\0");
			physicsComponent.colliderType_ = static_cast<ColliderType>(selectedIdx);

			size_t idx = 0;
			bool colliderParamsOpened = ImGui::TreeNodeEx((void*)(intptr_t)(idx), node_flags, "%s", "Collider Params");
			if (colliderParamsOpened)
			{
				float x, y, z, w;
				std::string xInput = std::to_string(physicsComponent.colliderParams_.x);
				if (ImGui::InputText("x", &xInput, input_text_flags))
				{
					if (!(xInput.empty()))
					{
						x = std::stof(xInput);
						physicsComponent.colliderParams_.x = x;
					}
				}
				std::string yInput = std::to_string(physicsComponent.colliderParams_.y);
				if (ImGui::InputText("y", &yInput, input_text_flags))
				{
					if (!(yInput.empty()))
					{
						y = std::stof(yInput);
						physicsComponent.colliderParams_.y = y;
					}
				}
				std::string zInput = std::to_string(physicsComponent.colliderParams_.z);
				if (ImGui::InputText("z", &zInput, input_text_flags))
				{
					if (!(zInput.empty()))
					{
						z = std::stof(zInput);
						physicsComponent.colliderParams_.z = z;
					}
				}
				std::string wInput = std::to_string(physicsComponent.colliderParams_.w);
				if (ImGui::InputText("w", &wInput, input_text_flags))
				{
					if (!(wInput.empty()))
					{
						w = std::stof(wInput);
						physicsComponent.colliderParams_.w = w;
					}
				}
				ImGui::TreePop();
			}
			++idx;
			switch (selectedIdx)
			{
				case 0:
				{
					if (physicsComponent.colliderParams_.x <= 0.0f)
					{
						physicsComponent.colliderParams_.x = 1.0f;
					}
					break;
				}
				case 1:
				{
					if (physicsComponent.colliderParams_.x <= 0.0f)
					{
						physicsComponent.colliderParams_.x = 1.0f;
					}
					if (physicsComponent.colliderParams_.y <= 0.0f)
					{
						physicsComponent.colliderParams_.y = 1.0f;
					}
					if (physicsComponent.colliderParams_.z <= 0.0f)
					{
						physicsComponent.colliderParams_.z = 1.0f;
					}
					if (physicsComponent.colliderParams_.w < 0.0f)
					{
						physicsComponent.colliderParams_.w = 0.0f;
					}
					break;
				}
				case 2:
				{
					if (physicsComponent.colliderParams_.x <= 0.0f)
					{
						physicsComponent.colliderParams_.x = 1.0f;
					}
					if (physicsComponent.colliderParams_.y <= 0.0f)
					{
						physicsComponent.colliderParams_.y = 1.0f;
					}
					break;
				}
				case 3:
				{
					if (physicsComponent.colliderParams_.x <= 0.0f)
					{
						physicsComponent.colliderParams_.x = 1.0f;
					}
					if (physicsComponent.colliderParams_.y <= 0.0f)
					{
						physicsComponent.colliderParams_.y = 1.0f;
					}
					if (physicsComponent.colliderParams_.z <= 0.0f)
					{
						physicsComponent.colliderParams_.z = 1.0f;
					}
					break;
				}
				case 4:
				{
					if (physicsComponent.colliderParams_.x <= 0.0f)
					{
						physicsComponent.colliderParams_.x = 1.0f;
					}
					if (physicsComponent.colliderParams_.y <= 0.0f)
					{
						physicsComponent.colliderParams_.y = 1.0f;
					}
					if (physicsComponent.colliderParams_.z < 0.0f)
					{
						physicsComponent.colliderParams_.z = 0.0f;
					}
					break;
				}
			}

			std::string massInput = std::to_string(physicsComponent.mass_);
			if (ImGui::InputText("Mass", &massInput, input_text_flags))
			{
				if (!(massInput.empty()))
				{
					mass = std::stof(massInput);
					if (mass >= 0.0f)
					{
						physicsComponent.mass_ = mass;
					}
				}
			}
			std::string frictionInput = std::to_string(physicsComponent.friction_);
			if (ImGui::InputText("Friction", &frictionInput, input_text_flags))
			{
				if (!(frictionInput.empty()))
				{
					friction = std::stof(frictionInput);
					if (friction >= 0.0f)
					{
						physicsComponent.friction_ = friction;
					}
				}
			}
			std::string restitutionInput = std::to_string(physicsComponent.restitution_);
			if (ImGui::InputText("Restitution", &restitutionInput, input_text_flags))
			{
				if (!(restitutionInput.empty()))
				{
					restitution = std::stof(restitutionInput);
					if (restitution >= 0.0f)
					{
						physicsComponent.restitution_ = restitution;
					}
				}
			}

			hasOffsetCenterOfMassSelection = physicsComponent.hasOffsetCenterOfMass_;
			ImGui::RadioButton("Offset Center of Mass Off", &hasOffsetCenterOfMassSelection, 0);
			ImGui::RadioButton("Offset Center of Mass On", &hasOffsetCenterOfMassSelection, 1);
			physicsComponent.hasOffsetCenterOfMass_ = hasOffsetCenterOfMassSelection;
			isSensorSelection = physicsComponent.isSensor_;
			ImGui::RadioButton("Is Sensor Off", &isSensorSelection, 0);
			ImGui::RadioButton("Is Sensor On", &isSensorSelection, 1);
			physicsComponent.isSensor_ = isSensorSelection;

			bool centerOfMassOffsetOpened = ImGui::TreeNodeEx((void*)(intptr_t)(idx), node_flags, "%s", "Center of Mass Offset");
			if (centerOfMassOffsetOpened)
			{
				float x, y, z;
				std::string xInput = std::to_string(physicsComponent.centerOfMassOffset_.x);
				if (ImGui::InputText("x", &xInput, input_text_flags))
				{
					if (!(xInput.empty()))
					{
						x = std::stof(xInput);
						physicsComponent.centerOfMassOffset_.x = x;
					}
				}
				std::string yInput = std::to_string(physicsComponent.centerOfMassOffset_.y);
				if (ImGui::InputText("y", &yInput, input_text_flags))
				{
					if (!(yInput.empty()))
					{
						y = std::stof(yInput);
						physicsComponent.centerOfMassOffset_.y = y;
					}
				}
				std::string zInput = std::to_string(physicsComponent.centerOfMassOffset_.z);
				if (ImGui::InputText("z", &zInput, input_text_flags))
				{
					if (!(zInput.empty()))
					{
						z = std::stof(zInput);
						physicsComponent.centerOfMassOffset_.z = z;
					}
				}
				ImGui::TreePop();
			}
			++idx;

			JPH::EMotionType motionType = physicsComponent.motionType_;
			selectedIdx = static_cast<int>(motionType);
			ImGui::Combo("Motion Type", &selectedIdx, "Static\0Kinematic\0Dynamic\0\0");
			physicsComponent.motionType_ = static_cast<JPH::EMotionType>(selectedIdx);

			bool velocityOpened = ImGui::TreeNodeEx((void*)(intptr_t)(idx), node_flags, "%s", "Velocity");
			if (velocityOpened)
			{
				float x, y, z;
				std::string xInput = std::to_string(physicsComponent.velocity_.x);
				if (ImGui::InputText("x", &xInput, input_text_flags))
				{
					if (!(xInput.empty()))
					{
						x = std::stof(xInput);
						physicsComponent.velocity_.x = x;
					}
				}
				std::string yInput = std::to_string(physicsComponent.velocity_.y);
				if (ImGui::InputText("y", &yInput, input_text_flags))
				{
					if (!(yInput.empty()))
					{
						y = std::stof(yInput);
						physicsComponent.velocity_.y = y;
					}
				}
				std::string zInput = std::to_string(physicsComponent.velocity_.z);
				if (ImGui::InputText("z", &zInput, input_text_flags))
				{
					if (!(zInput.empty()))
					{
						z = std::stof(zInput);
						physicsComponent.velocity_.z = z;
					}
				}
				ImGui::TreePop();
			}
			++idx;
			bool angularVelocityOpened = ImGui::TreeNodeEx((void*)(intptr_t)(idx), node_flags, "%s", "Angular Velocity");
			if (angularVelocityOpened)
			{
				float x, y, z;
				std::string xInput = std::to_string(physicsComponent.angularVelocity_.x);
				if (ImGui::InputText("x", &xInput, input_text_flags))
				{
					if (!(xInput.empty()))
					{
						x = std::stof(xInput);
						physicsComponent.angularVelocity_.x = x;
					}
				}
				std::string yInput = std::to_string(physicsComponent.angularVelocity_.y);
				if (ImGui::InputText("y", &yInput, input_text_flags))
				{
					if (!(yInput.empty()))
					{
						y = std::stof(yInput);
						physicsComponent.angularVelocity_.y = y;
					}
				}
				std::string zInput = std::to_string(physicsComponent.angularVelocity_.z);
				if (ImGui::InputText("z", &zInput, input_text_flags))
				{
					if (!(zInput.empty()))
					{
						z = std::stof(zInput);
						physicsComponent.angularVelocity_.z = z;
					}
				}
				ImGui::TreePop();
			}

			ImGui::Separator();
			ImGui::Button("Create", ImVec2(0, 0));
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				game_->GetRegistry().emplace<PhysicsComponent>(currentEntity, physicsComponent);
				physicsComponent = {};
				open = false;
			}
			ImGui::SameLine();
			ImGui::Button("Cancel", ImVec2(0, 0));
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				physicsComponent = {};
				open = false;
			}
			ImGui::End();
			break;
		}
		case 2:
		{
			ImGui::Begin("Create PhysicsCharacterComponent", nullptr, flags);
			if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && editor_->lmbDownLastFrame)
			{
				ImGui::SetWindowFocus();
			}

			float maxSlopeAngle, collisionTolerance, yaw, pitch, speed, jumpSpeed, sensitivityX, sensitivityY, mass, friction, restitution;
			int hasOffsetCenterOfMass, controlMovementDuringJump, isLMBActivated;
			ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
			std::string maxSlopeAngleInput = std::to_string(characterComponent.maxSlopeAngle_);
			if (ImGui::InputText("Max Slope Angle", &maxSlopeAngleInput, input_text_flags))
			{
				if (!(maxSlopeAngleInput.empty()))
				{
					maxSlopeAngle = std::stof(maxSlopeAngleInput);
					characterComponent.maxSlopeAngle_ = maxSlopeAngle;
				}
			}
			std::string collisionToleranceInput = std::to_string(characterComponent.collisionTolerance_);
			if (ImGui::InputText("Collision Tolerance", &collisionToleranceInput, input_text_flags))
			{
				if (!(collisionToleranceInput.empty()))
				{
					collisionTolerance = std::stof(collisionToleranceInput);
					characterComponent.collisionTolerance_ = collisionTolerance;
				}
			}
			controlMovementDuringJump = characterComponent.controlMovementDuringJump_;
			ImGui::RadioButton("Control Movement During Jump Off", &controlMovementDuringJump, 0);
			ImGui::RadioButton("Control Movement During Jump On", &controlMovementDuringJump, 1);
			characterComponent.controlMovementDuringJump_ = controlMovementDuringJump;

			std::string yawInput = std::to_string(characterComponent.yaw_);
			if (ImGui::InputText("Yaw", &yawInput, input_text_flags))
			{
				if (!(yawInput.empty()))
				{
					yaw = std::stof(yawInput);
					characterComponent.yaw_ = yaw;
				}
			}
			std::string pitchInput = std::to_string(characterComponent.pitch_);
			if (ImGui::InputText("Pitch", &pitchInput, input_text_flags))
			{
				if (!(pitchInput.empty()))
				{
					pitch = std::stof(pitchInput);
					characterComponent.pitch_ = pitch;
				}
			}
			std::string speedInput = std::to_string(characterComponent.speed_);
			if (ImGui::InputText("Speed", &speedInput, input_text_flags))
			{
				if (!(speedInput.empty()))
				{
					speed = std::stof(speedInput);
					if (speed > 0.0f)
					{
						characterComponent.speed_ = speed;
					}
				}
			}
			std::string jumpSpeedInput = std::to_string(characterComponent.jumpSpeed_);
			if (ImGui::InputText("Jump Speed", &jumpSpeedInput, input_text_flags))
			{
				if (!(jumpSpeedInput.empty()))
				{
					jumpSpeed = std::stof(jumpSpeedInput);
					if (jumpSpeed > 0.0f)
					{
						characterComponent.jumpSpeed_ = jumpSpeed;
					}
				}
			}
			std::string sensitivityXInput = std::to_string(characterComponent.sensitivityX_);
			if (ImGui::InputText("Sensitivity X", &sensitivityXInput, input_text_flags))
			{
				if (!(sensitivityXInput.empty()))
				{
					sensitivityX = std::stof(sensitivityXInput);
					if (sensitivityX > 0.0f)
					{
						characterComponent.sensitivityX_ = sensitivityX;
					}
				}
			}
			std::string sensitivityYInput = std::to_string(characterComponent.sensitivityY_);
			if (ImGui::InputText("Sensitivity Y", &sensitivityYInput, input_text_flags))
			{
				if (!(sensitivityYInput.empty()))
				{
					sensitivityY = std::stof(sensitivityYInput);
					if (sensitivityY > 0.0f)
					{
						characterComponent.sensitivityY_ = sensitivityY;
					}
				}
			}
			isLMBActivated = characterComponent.isLMBActivated_;
			ImGui::RadioButton("Is LMB Activated Off", &isLMBActivated, 0);
			ImGui::RadioButton("Is LMB Activated On", &isLMBActivated, 1);
			characterComponent.isLMBActivated_ = isLMBActivated;

			ColliderType colliderType = characterComponent.colliderType_;
			int selectedIdx = static_cast<int>(colliderType);
			ImGui::Combo("Collider Type", &selectedIdx, "SphereCollider\0BoxCollider\0CapsuleCollider\0TaperedCapsuleCollider\0CylinderCollider\0\0");
			characterComponent.colliderType_ = static_cast<ColliderType>(selectedIdx);
			size_t idx = 0;
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow;
			bool colliderParamsOpened = ImGui::TreeNodeEx((void*)(intptr_t)(idx), node_flags, "%s", "Collider Params");
			if (colliderParamsOpened)
			{
				float x, y, z, w;
				std::string xInput = std::to_string(characterComponent.colliderParams_.x);
				if (ImGui::InputText("x", &xInput, input_text_flags))
				{
					if (!(xInput.empty()))
					{
						x = std::stof(xInput);
						characterComponent.colliderParams_.x = x;
					}
				}
				std::string yInput = std::to_string(characterComponent.colliderParams_.y);
				if (ImGui::InputText("y", &yInput, input_text_flags))
				{
					if (!(yInput.empty()))
					{
						y = std::stof(yInput);
						characterComponent.colliderParams_.y = y;
					}
				}
				std::string zInput = std::to_string(characterComponent.colliderParams_.z);
				if (ImGui::InputText("z", &zInput, input_text_flags))
				{
					if (!(zInput.empty()))
					{
						z = std::stof(zInput);
						characterComponent.colliderParams_.z = z;
					}
				}
				std::string wInput = std::to_string(characterComponent.colliderParams_.w);
				if (ImGui::InputText("w", &wInput, input_text_flags))
				{
					if (!(wInput.empty()))
					{
						w = std::stof(wInput);
						characterComponent.colliderParams_.w = w;
					}
				}
				ImGui::TreePop();
			}
			++idx;
			switch (selectedIdx)
			{
				case 0:
				{
					if (characterComponent.colliderParams_.x <= 0.0f)
					{
						characterComponent.colliderParams_.x = 1.0f;
					}
					break;
				}
				case 1:
				{
					if (characterComponent.colliderParams_.x <= 0.0f)
					{
						characterComponent.colliderParams_.x = 1.0f;
					}
					if (characterComponent.colliderParams_.y <= 0.0f)
					{
						characterComponent.colliderParams_.y = 1.0f;
					}
					if (characterComponent.colliderParams_.z <= 0.0f)
					{
						characterComponent.colliderParams_.z = 1.0f;
					}
					if (characterComponent.colliderParams_.w < 0.0f)
					{
						characterComponent.colliderParams_.w = 0.0f;
					}
					break;
				}
				case 2:
				{
					if (characterComponent.colliderParams_.x <= 0.0f)
					{
						characterComponent.colliderParams_.x = 1.0f;
					}
					if (characterComponent.colliderParams_.y <= 0.0f)
					{
						characterComponent.colliderParams_.y = 1.0f;
					}
					break;
				}
				case 3:
				{
					if (characterComponent.colliderParams_.x <= 0.0f)
					{
						characterComponent.colliderParams_.x = 1.0f;
					}
					if (characterComponent.colliderParams_.y <= 0.0f)
					{
						characterComponent.colliderParams_.y = 1.0f;
					}
					if (characterComponent.colliderParams_.z <= 0.0f)
					{
						characterComponent.colliderParams_.z = 1.0f;
					}
					break;
				}
				case 4:
				{
					if (characterComponent.colliderParams_.x <= 0.0f)
					{
						characterComponent.colliderParams_.x = 1.0f;
					}
					if (characterComponent.colliderParams_.y <= 0.0f)
					{
						characterComponent.colliderParams_.y = 1.0f;
					}
					if (characterComponent.colliderParams_.z < 0.0f)
					{
						characterComponent.colliderParams_.z = 0.0f;
					}
					break;
				}
			}

			std::string massInput = std::to_string(characterComponent.mass_);
			if (ImGui::InputText("Mass", &massInput, input_text_flags))
			{
				if (!(massInput.empty()))
				{
					mass = std::stof(massInput);
					if (mass >= 0.0f)
					{
						characterComponent.mass_ = mass;
					}
				}
			}
			std::string frictionInput = std::to_string(characterComponent.friction_);
			if (ImGui::InputText("Friction", &frictionInput, input_text_flags))
			{
				if (!(frictionInput.empty()))
				{
					friction = std::stof(frictionInput);
					if (friction >= 0.0f)
					{
						characterComponent.friction_ = friction;
					}
				}
			}
			std::string restitutionInput = std::to_string(characterComponent.restitution_);
			if (ImGui::InputText("Restitution", &restitutionInput, input_text_flags))
			{
				if (!(restitutionInput.empty()))
				{
					restitution = std::stof(restitutionInput);
					if (restitution >= 0.0f)
					{
						characterComponent.restitution_ = restitution;
					}
				}
			}
			hasOffsetCenterOfMass = characterComponent.hasOffsetCenterOfMass_;
			ImGui::RadioButton("Offset Center of Mass Off", &hasOffsetCenterOfMass, 0);
			ImGui::RadioButton("Offset Center of Mass On", &hasOffsetCenterOfMass, 1);
			characterComponent.hasOffsetCenterOfMass_ = hasOffsetCenterOfMass;

			bool velocityOpened = ImGui::TreeNodeEx((void*)(intptr_t)(idx), node_flags, "%s", "Velocity");
			if (velocityOpened)
			{
				float x, y, z;
				std::string xInput = std::to_string(characterComponent.velocity_.x);
				if (ImGui::InputText("x", &xInput, input_text_flags))
				{
					if (!(xInput.empty()))
					{
						x = std::stof(xInput);
						characterComponent.velocity_.x = x;
					}
				}
				std::string yInput = std::to_string(characterComponent.velocity_.y);
				if (ImGui::InputText("y", &yInput, input_text_flags))
				{
					if (!(yInput.empty()))
					{
						y = std::stof(yInput);
						characterComponent.velocity_.y = y;
					}
				}
				std::string zInput = std::to_string(characterComponent.velocity_.z);
				if (ImGui::InputText("z", &zInput, input_text_flags))
				{
					if (!(zInput.empty()))
					{
						z = std::stof(zInput);
						characterComponent.velocity_.z = z;
					}
				}
				ImGui::TreePop();
			}
			++idx;
			bool angularVelocityOpened = ImGui::TreeNodeEx((void*)(intptr_t)(idx), node_flags, "%s", "Angular Velocity");
			if (angularVelocityOpened)
			{
				float x, y, z;
				std::string xInput = std::to_string(characterComponent.angularVelocity_.x);
				if (ImGui::InputText("x", &xInput, input_text_flags))
				{
					if (!(xInput.empty()))
					{
						x = std::stof(xInput);
						characterComponent.angularVelocity_.x = x;
					}
				}
				std::string yInput = std::to_string(characterComponent.angularVelocity_.y);
				if (ImGui::InputText("y", &yInput, input_text_flags))
				{
					if (!(yInput.empty()))
					{
						y = std::stof(yInput);
						characterComponent.angularVelocity_.y = y;
					}
				}
				std::string zInput = std::to_string(characterComponent.angularVelocity_.z);
				if (ImGui::InputText("z", &zInput, input_text_flags))
				{
					if (!(zInput.empty()))
					{
						z = std::stof(zInput);
						characterComponent.angularVelocity_.z = z;
					}
				}
				ImGui::TreePop();
			}

			ImGui::Separator();
			ImGui::Button("Create", ImVec2(0, 0));
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				game_->GetRegistry().emplace<PhysicsCharacterComponent>(currentEntity, characterComponent);
				characterComponent = {};
				open = false;
			}
			ImGui::SameLine();
			ImGui::Button("Cancel", ImVec2(0, 0));
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				characterComponent = {};
				open = false;
			}
			ImGui::End();
			break;
		}
		case 3:
		{
			ImGui::Begin("Create CameraComponent", nullptr, flags);
			if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && editor_->lmbDownLastFrame)
			{
				ImGui::SetWindowFocus();
			}

			float orthographicWidth, orthographicHeight, fov, aspectRatio, nearPlane, farPlane;
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow;
			ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
			size_t idx = 0;
			bool offsetOpened = ImGui::TreeNodeEx((void*)(intptr_t)(idx), node_flags, "%s", "Offset");
			if (offsetOpened)
			{
				float x, y, z;
				std::string xInput = std::to_string(cameraComponent.offset.x);
				if (ImGui::InputText("x", &xInput, input_text_flags))
				{
					if (!(xInput.empty()))
					{
						x = std::stof(xInput);
						cameraComponent.offset.x = x;
						cameraComponent.initialOffset.x = x;
					}
				}
				std::string yInput = std::to_string(cameraComponent.offset.y);
				if (ImGui::InputText("y", &yInput, input_text_flags))
				{
					if (!(yInput.empty()))
					{
						y = std::stof(yInput);
						cameraComponent.offset.y = y;
						cameraComponent.initialOffset.y = y;
					}
				}
				std::string zInput = std::to_string(cameraComponent.offset.z);
				if (ImGui::InputText("z", &zInput, input_text_flags))
				{
					if (!(zInput.empty()))
					{
						z = std::stof(zInput);
						cameraComponent.offset.z = z;
						cameraComponent.initialOffset.z = z;
					}
				}
				ImGui::TreePop();
			}
			++idx;
			bool upOpened = ImGui::TreeNodeEx((void*)(intptr_t)(idx), node_flags, "%s", "Up");
			if (upOpened)
			{
				float x, y, z;
				std::string xInput = std::to_string(cameraComponent.up.x);
				if (ImGui::InputText("x", &xInput, input_text_flags))
				{
					if (!(xInput.empty()))
					{
						x = std::stof(xInput);
						cameraComponent.up.x = x;
					}
				}
				std::string yInput = std::to_string(cameraComponent.up.y);
				if (ImGui::InputText("y", &yInput, input_text_flags))
				{
					if (!(yInput.empty()))
					{
						y = std::stof(yInput);
						cameraComponent.up.y = y;
					}
				}
				std::string zInput = std::to_string(cameraComponent.up.z);
				if (ImGui::InputText("z", &zInput, input_text_flags))
				{
					if (!(zInput.empty()))
					{
						z = std::stof(zInput);
						cameraComponent.up.z = z;
					}
				}
				ImGui::TreePop();
			}
			++idx;
			bool forwardOpened = ImGui::TreeNodeEx((void*)(intptr_t)(idx), node_flags, "%s", "Forward");
			if (forwardOpened)
			{
				float x, y, z;
				std::string xInput = std::to_string(cameraComponent.forward.x);
				if (ImGui::InputText("x", &xInput, input_text_flags))
				{
					if (!(xInput.empty()))
					{
						x = std::stof(xInput);
						cameraComponent.forward.x = x;
					}
				}
				std::string yInput = std::to_string(cameraComponent.forward.y);
				if (ImGui::InputText("y", &yInput, input_text_flags))
				{
					if (!(yInput.empty()))
					{
						y = std::stof(yInput);
						cameraComponent.forward.y = y;
					}
				}
				std::string zInput = std::to_string(cameraComponent.forward.z);
				if (ImGui::InputText("z", &zInput, input_text_flags))
				{
					if (!(zInput.empty()))
					{
						z = std::stof(zInput);
						cameraComponent.forward.z = z;
					}
				}
				ImGui::TreePop();
			}

			int isOrthographic = cameraComponent.isOrthographic;
			ImGui::RadioButton("Is Orthographic Off", &isOrthographic, 0);
			ImGui::RadioButton("Is Orthographic On", &isOrthographic, 1);
			cameraComponent.isOrthographic = isOrthographic;

			std::string orthographicWidthInput = std::to_string(cameraComponent.orthographicWidth);
			if (ImGui::InputText("Orthographic Width", &orthographicWidthInput, input_text_flags))
			{
				if (!(orthographicWidthInput.empty()))
				{
					orthographicWidth = std::stof(orthographicWidthInput);
					if (orthographicWidth > 0.0f)
					{
						cameraComponent.orthographicWidth = orthographicWidth;
					}
				}
			}

			std::string orthographicHeightInput = std::to_string(cameraComponent.orthographicHeight);
			if (ImGui::InputText("Orthographic Height", &orthographicHeightInput, input_text_flags))
			{
				if (!(orthographicHeightInput.empty()))
				{
					orthographicHeight = std::stof(orthographicHeightInput);
					if (orthographicHeight > 0.0f)
					{
						cameraComponent.orthographicHeight = orthographicHeight;
					}
				}
			}

			std::string fovInput = std::to_string(cameraComponent.fov);
			if (ImGui::InputText("FOV", &fovInput, input_text_flags))
			{
				if (!(fovInput.empty()))
				{
					fov = std::stof(fovInput);
					if (fov > 0.0f)
					{
						cameraComponent.fov = fov;
					}
				}
			}

			std::string aspectRatioInput = std::to_string(cameraComponent.aspectRatio);
			if (ImGui::InputText("Aspect Ratio", &aspectRatioInput, input_text_flags))
			{
				if (!(aspectRatioInput.empty()))
				{
					aspectRatio = std::stof(aspectRatioInput);
					if (aspectRatio > 0.0f)
					{
						cameraComponent.aspectRatio = aspectRatio;
					}
				}
			}

			std::string nearPlaneInput = std::to_string(cameraComponent.nearPlane);
			if (ImGui::InputText("Near Plane", &nearPlaneInput, input_text_flags))
			{
				if (!(nearPlaneInput.empty()))
				{
					nearPlane = std::stof(nearPlaneInput);
					if (nearPlane > 0.0f)
					{
						cameraComponent.nearPlane = nearPlane;
					}
				}
			}

			std::string farPlaneInput = std::to_string(cameraComponent.farPlane);
			if (ImGui::InputText("Far Plane", &farPlaneInput, input_text_flags))
			{
				if (!(farPlaneInput.empty()))
				{
					farPlane = std::stof(farPlaneInput);
					if (farPlane > 0.0f)
					{
						cameraComponent.farPlane = farPlane;
					}
				}
			}

			ImGui::Separator();
			ImGui::Button("Create", ImVec2(0, 0));
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				game_->GetRegistry().emplace<CameraComponent>(currentEntity, cameraComponent);
				cameraComponent = {};
				open = false;
			}
			ImGui::SameLine();
			ImGui::Button("Cancel", ImVec2(0, 0));
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				cameraComponent = {};
				open = false;
			}
			ImGui::End();
			break;
		}
		case 4:
		{
			ImGui::Begin("Create LightComponent", nullptr, flags);
			if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && editor_->lmbDownLastFrame)
			{
				ImGui::SetWindowFocus();
			}

			float intensity;
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow;
			ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
			size_t idx = 0;
			LightType lightType = lightComponent.lightType;
			int selectedIdx = lightType;
			ImGui::Combo("Light Type", &selectedIdx, "Directional\0Point\0Spot\0\0");
			lightComponent.lightType = static_cast<LightType>(selectedIdx);
			bool offsetOpened = ImGui::TreeNodeEx((void*)(intptr_t)(idx), node_flags, "%s", "Offset");
			if (offsetOpened)
			{
				float x, y, z;
				std::string xInput = std::to_string(lightComponent.offset.x);
				if (ImGui::InputText("x", &xInput, input_text_flags))
				{
					if (!(xInput.empty()))
					{
						x = std::stof(xInput);
						lightComponent.offset.x = x;
					}
				}
				std::string yInput = std::to_string(lightComponent.offset.y);
				if (ImGui::InputText("y", &yInput, input_text_flags))
				{
					if (!(yInput.empty()))
					{
						y = std::stof(yInput);
						lightComponent.offset.y = y;
					}
				}
				std::string zInput = std::to_string(lightComponent.offset.z);
				if (ImGui::InputText("z", &zInput, input_text_flags))
				{
					if (!(zInput.empty()))
					{
						z = std::stof(zInput);
						lightComponent.offset.z = z;
					}
				}
				ImGui::TreePop();
			}
			++idx;
			bool directionOpened = ImGui::TreeNodeEx((void*)(intptr_t)(idx), node_flags, "%s", "Direction");
			if (directionOpened)
			{
				float x, y, z;
				std::string xInput = std::to_string(lightComponent.direction.x);
				if (ImGui::InputText("x", &xInput, input_text_flags))
				{
					if (!(xInput.empty()))
					{
						x = std::stof(xInput);
						lightComponent.direction.x = x;
					}
				}
				std::string yInput = std::to_string(lightComponent.direction.y);
				if (ImGui::InputText("y", &yInput, input_text_flags))
				{
					if (!(yInput.empty()))
					{
						y = std::stof(yInput);
						lightComponent.direction.y = y;
					}
				}
				std::string zInput = std::to_string(lightComponent.direction.z);
				if (ImGui::InputText("z", &zInput, input_text_flags))
				{
					if (!(zInput.empty()))
					{
						z = std::stof(zInput);
						lightComponent.direction.z = z;
					}
				}
				ImGui::TreePop();
			}
			++idx;
			bool colorOpened = ImGui::TreeNodeEx((void*)(intptr_t)(idx), node_flags, "%s", "Color");
			if (colorOpened)
			{
				float x, y, z;
				std::string xInput = std::to_string(lightComponent.color.x);
				if (ImGui::InputText("x", &xInput, input_text_flags))
				{
					if (!(xInput.empty()))
					{
						x = std::stof(xInput);
						if (x >= 0.0f && x <= 1.0f)
						{
							lightComponent.color.x = x;
						}
					}
				}
				std::string yInput = std::to_string(lightComponent.color.y);
				if (ImGui::InputText("y", &yInput, input_text_flags))
				{
					if (!(yInput.empty()))
					{
						y = std::stof(yInput);
						if (y >= 0.0f && y <= 1.0f)
						{
							lightComponent.color.y = y;
						}
					}
				}
				std::string zInput = std::to_string(lightComponent.color.z);
				if (ImGui::InputText("z", &zInput, input_text_flags))
				{
					if (!(zInput.empty()))
					{
						z = std::stof(zInput);
						if (z >= 0.0f && z <= 1.0f)
						{
							lightComponent.color.z = z;
						}
					}
				}
				ImGui::TreePop();
			}
			std::string intensityInput = std::to_string(lightComponent.intensity);
			if (ImGui::InputText("Intensity", &intensityInput, input_text_flags))
			{
				if (!(intensityInput.empty()))
				{
					intensity = std::stof(intensityInput);
					if (intensity >= 0.0f)
					{
						lightComponent.intensity = intensity;
					}
				}
			}
			int castsShadows = lightComponent.castsShadows;
			ImGui::RadioButton("Casts Shadows Off", &castsShadows, 0);
			ImGui::RadioButton("Casts Shadows On", &castsShadows, 1);
			lightComponent.castsShadows = castsShadows;

			ImGui::Separator();
			ImGui::Button("Create", ImVec2(0, 0));
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				LightInitSystem::IsDirty = true;
				game_->GetRegistry().emplace<LightComponent>(currentEntity, lightComponent);
				lightComponent = {};
				open = false;
			}
			ImGui::SameLine();
			ImGui::Button("Cancel", ImVec2(0, 0));
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				lightComponent = {};
				open = false;
			}
			ImGui::End();
			break;
		}
		case 6:
		{
			ImGui::Begin("Create SoundComponent", nullptr, flags);
			if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && editor_->lmbDownLastFrame)
			{
				ImGui::SetWindowFocus();
			}

			int is3D, isLooping, isStreaming;
			float volume;
			is3D = soundComponent.is3D;
			ImGui::RadioButton("Is 3D Off", &is3D, 0);
			ImGui::RadioButton("Is 3D On", &is3D, 1);
			soundComponent.is3D = is3D;
			isLooping = soundComponent.isLooping;
			ImGui::RadioButton("Is Looping Off", &isLooping, 0);
			ImGui::RadioButton("Is Looping On", &isLooping, 1);
			soundComponent.isLooping = isLooping;
			isStreaming = soundComponent.isStreaming;
			ImGui::RadioButton("Is Streaming Off", &isStreaming, 0);
			ImGui::RadioButton("Is Streaming On", &isStreaming, 1);
			soundComponent.isStreaming = isStreaming;
			ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
			std::string volumeInput = std::to_string(soundComponent.volume);
			if (ImGui::InputText("Volume", &volumeInput, input_text_flags))
			{
				if (!(volumeInput.empty()))
				{
					volume = std::stof(volumeInput);
					if (volume >= 0.0f)
					{
						soundComponent.volume = volume;
					}
				}
			}
			size_t idx = 0;
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow;
			bool locationOpened = ImGui::TreeNodeEx((void*)(intptr_t)(idx), node_flags, "%s", "Location");
			if (locationOpened)
			{
				float x, y, z;
				std::string xInput = std::to_string(soundComponent.location.x);
				if (ImGui::InputText("x", &xInput, input_text_flags))
				{
					if (!(xInput.empty()))
					{
						x = std::stof(xInput);
						soundComponent.location.x = x;
					}
				}
				std::string yInput = std::to_string(soundComponent.location.y);
				if (ImGui::InputText("y", &yInput, input_text_flags))
				{
					if (!(yInput.empty()))
					{
						y = std::stof(yInput);
						soundComponent.location.y = y;
					}
				}
				std::string zInput = std::to_string(soundComponent.location.z);
				if (ImGui::InputText("z", &zInput, input_text_flags))
				{
					if (!(zInput.empty()))
					{
						z = std::stof(zInput);
						soundComponent.location.z = z;
					}
				}
				ImGui::TreePop();
			}
			ImGui::Separator();
			ImGui::Button("Create", ImVec2(0, 0));
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				game_->GetRegistry().emplace<SoundComponent>(currentEntity, soundComponent);
				soundComponent = {};
				open = false;
			}
			ImGui::SameLine();
			ImGui::Button("Cancel", ImVec2(0, 0));
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				soundComponent = {};
				open = false;
			}
			ImGui::End();
			break;
		}
	}
}
