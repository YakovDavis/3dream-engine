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
				game_->GetRegistry().emplace<PhysicsComponent>(currentEntity, characterComponent);
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
	}
}
