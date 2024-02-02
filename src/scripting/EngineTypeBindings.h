#pragma once

#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Components/PhysicsCharacterComponent.h"
#include "D3E/Components/PhysicsComponent.h"
#include "D3E/Components/TransformComponent.h"
#include "D3E/Components/render/LightComponent.h"
#include "D3E/Debug.h"
#include "D3E/ai/Action.h"
#include "D3E/ai/Goal.h"
#include "D3E/time/time.h"
#include "Jolt/Physics/Body/Body.h"
#include "SimpleMath.h"
#include "navigation/NavigationManager.h"
#include "render/RenderUtils.h"`
#include "scripting/LuaECSAdapter.h"
#include "scripting/type_adapters/AiAgentAdapter.h"
#include "scripting/type_adapters/InfoAdapter.h"
#include "scripting/type_adapters/InputDeviceAdapter.h"
#include "scripting/type_adapters/PhysicsActivationAdapter.h"
#include "sol/sol.hpp"
#include "utils/ECSUtils.h"

namespace D3E
{
	using namespace DirectX::SimpleMath;

	static void BindComponentType(sol::state& state)
	{
		state["ComponentType"] = state.create_table_with(
			"Transform", ComponentType::kTransformComponent, "Physics",
			ComponentType::kPhysicsComponent, "Script",
			ComponentType::kScriptComponent, "PhysicsCharacter",
			ComponentType::kPhysicsCharacterComponent, "Info",
			ComponentType::kObjectInfoComponent, "Sound",
			ComponentType::kSoundComponent, "Light",
			ComponentType::kLightComponent, "Camera",
			ComponentType::kCameraComponent, "StaticMesh",
			ComponentType::kStaticMeshComponent, "AiAgent",
			ComponentType::kAiAgentComponent);
	}

	static void BindMatrix(sol::state& state)
	{
		auto matrixType = state.new_usertype<Matrix>(
			"Martix",
			sol::constructors<Matrix(), Matrix(const Matrix&),
		                      Matrix(const Vector3&, const Vector3&,
		                             const Vector3&)>(),
			sol::meta_function::equal_to, &Matrix::operator==,
			sol::meta_function::addition, &Matrix::operator+,
			sol::meta_function::subtraction, &Matrix::operator-,
			sol::meta_function::multiplication,
			sol::overload(
				sol::resolve<Matrix(const Matrix&, const Matrix&)>(::operator*),
				sol::resolve<Matrix(const Matrix&, float)>(::operator*),
				sol::resolve<Matrix(float, const Matrix&)>(::operator*)),
			sol::meta_function::division,
			sol::overload(
				sol::resolve<Matrix(const Matrix&, const Matrix&)>(::operator/),
				sol::resolve<Matrix(const Matrix&, float)>(::operator/)));

		matrixType["transpose"] =
			sol::resolve<Matrix() const>(&Matrix::Transpose);
		matrixType["invert"] = sol::resolve<Matrix() const>(&Matrix::Invert);
		matrixType["determinant"] = &Matrix::Determinant;
		matrixType["to_euler"] = &Matrix::ToEuler;
		matrixType["create_translation"] = sol::overload(
			sol::resolve<const Vector3&>(&Matrix::CreateTranslation),
			sol::resolve<float, float, float>(&Matrix::CreateTranslation));
		matrixType["create_scale"] = sol::overload(
			sol::resolve<const Vector3&>(&Matrix::CreateScale),
			sol::resolve<float, float, float>(&Matrix::CreateScale),
			sol::resolve<Matrix(float)>(&Matrix::CreateScale));
		matrixType["create_rotation_x"] = &Matrix::CreateRotationX;
		matrixType["create_rotation_y"] = &Matrix::CreateRotationY;
		matrixType["create_rotation_z"] = &Matrix::CreateRotationZ;
		matrixType["create_from_axis_angle"] = &Matrix::CreateFromAxisAngle;
		matrixType["create_from_quaternion"] = &Matrix::CreateFromQuaternion;
		matrixType["create_from_yaw_pith_roll"] = sol::overload(
			sol::resolve<float, float, float>(&Matrix::CreateFromYawPitchRoll),
			sol::resolve<const Vector3&>(&Matrix::CreateFromYawPitchRoll));
		matrixType["transform"] =
			sol::resolve<Matrix(const Matrix&, const Quaternion&)>(
				&Matrix::Transform);
	}

	static void BindVector(sol::state& state)
	{
		auto vector3Type = state.new_usertype<Vector3>(
			"Vector3",
			sol::constructors<Vector3(), Vector3(float, float, float),
		                      Vector3(const Vector3&)>(),
			sol::meta_function::addition,
			sol::resolve<const Vector3&, const Vector3&>(::operator+),
			sol::meta_function::subtraction,
			sol::resolve<const Vector3&, const Vector3&>(::operator-),
			sol::meta_function::equal_to, &Vector3::operator==,
			sol::meta_function::multiplication,
			sol::overload(
				sol::resolve<const Vector3&, const Vector3&>(::operator*),
				sol::resolve<const Vector3&, float>(::operator*),
				sol::resolve<float, const Vector3&>(::operator*)),
			sol::meta_function::division,
			sol::overload(
				sol::resolve<const Vector3&, const Vector3&>(::operator/),
				sol::resolve<const Vector3&, float>(::operator/)));

		vector3Type["x"] = &Vector3::x;
		vector3Type["y"] = &Vector3::y;
		vector3Type["z"] = &Vector3::z;
		vector3Type["length"] = &Vector3::Length;
		vector3Type["length_squared"] = &Vector3::LengthSquared;
		vector3Type["distance"] = &Vector3::Distance;
		vector3Type["distance_squared"] = &Vector3::DistanceSquared;
		vector3Type["min"] =
			sol::resolve<Vector3(const Vector3&, const Vector3&)>(
				&Vector3::Min);
		vector3Type["max"] =
			sol::resolve<Vector3(const Vector3&, const Vector3&)>(
				&Vector3::Max);
		vector3Type["transform"] = sol::overload(
			sol::resolve<Vector3(const Vector3&, const Matrix&)>(
				&Vector3::Transform),
			sol::resolve<Vector3(const Vector3&, const Quaternion&)>(
				&Vector3::Transform));
		vector3Type["reflect"] =
			sol::resolve<Vector3(const Vector3&, const Vector3&)>(
				&Vector3::Reflect);
		vector3Type["in_bounds"] = &Vector3::InBounds;
		vector3Type["dot"] = &Vector3::Dot;
		vector3Type["cross"] =
			sol::resolve<Vector3(const Vector3&) const>(&Vector3::Cross);
		vector3Type["normalize"] = sol::resolve<void()>(&Vector3::Normalize);
		vector3Type["clamp"] =
			sol::resolve<void(const Vector3&, const Vector3&)>(&Vector3::Clamp);
	}

	static void BindQuaternion(sol::state& state)
	{
		auto quaternionType = state.new_usertype<Quaternion>(
			"Quaternion",
			sol::constructors<Quaternion(), Quaternion(const Quaternion&),
		                      Quaternion(float, float, float, float),
		                      Quaternion(const Vector3&, float),
		                      Quaternion(const Vector4&)>(),
			sol::meta_function::equal_to, &Quaternion::operator==,
			sol::meta_function::addition, &Quaternion::operator+,
			sol::meta_function::subtraction, &Quaternion::operator-,
			sol::meta_function::multiplication,
			sol::overload(
				sol::resolve<const Quaternion&, const Quaternion&>(::operator*),
				sol::resolve<const Quaternion&, float>(::operator*),
				sol::resolve<float, const Quaternion&>(::operator*)),
			sol::meta_function::division,
			sol::resolve<const Quaternion&, const Quaternion&>(::operator/));

		quaternionType["x"] = &Quaternion::x;
		quaternionType["y"] = &Quaternion::y;
		quaternionType["z"] = &Quaternion::z;
		quaternionType["w"] = &Quaternion::w;
		quaternionType["length"] = &Quaternion::Length;
		quaternionType["length_squared"] = &Quaternion::LengthSquared;
		quaternionType["normalize"] =
			sol::resolve<void()>(&Quaternion::Normalize);
		quaternionType["conjugate"] =
			sol::resolve<void()>(&Quaternion::Conjugate);
		quaternionType["dot"] = &Quaternion::Dot;
		quaternionType["rotate_towards"] =
			sol::resolve<void(const Quaternion&, float)>(
				&Quaternion::RotateTowards);
		quaternionType["to_euler"] = &Quaternion::ToEuler;
		quaternionType["create_from_axis_angle"] =
			&Quaternion::CreateFromAxisAngle;
		quaternionType["create_from_yaw_pitch_roll"] = sol::overload(
			sol::resolve<float, float, float>(
				&Quaternion::CreateFromYawPitchRoll),
			sol::resolve<const Vector3&>(&Quaternion::CreateFromYawPitchRoll));
		quaternionType["create_from_rotation_matrix"] =
			&Quaternion::CreateFromRotationMatrix;
		quaternionType["concatenate"] =
			sol::resolve<Quaternion(const Quaternion&, const Quaternion&)>(
				&Quaternion::Concatenate);
		quaternionType["from_to_rotation"] =
			sol::resolve<Quaternion(const Vector3&, const Vector3&)>(
				&Quaternion::FromToRotation);
		quaternionType["look_rotation"] =
			sol::resolve<Quaternion(const Vector3&, const Vector3&)>(
				&Quaternion::LookRotation);
	}

	static void BindTransformCompoent(sol::state& state)
	{
		auto transformComponentType =
			state.new_usertype<TransformComponent>("TransformComponent");

		transformComponentType["location"] = &D3E::TransformComponent::position;
		transformComponentType["rotation"] = &D3E::TransformComponent::rotation;
		transformComponentType["scale"] = &D3E::TransformComponent::scale;
	}

	static void BindPhysicsComponent(sol::state& state)
	{
		auto physicsComponentType =
			state.new_usertype<PhysicsComponent>("PhysicsComponent");

		physicsComponentType["mass"] = &PhysicsComponent::mass_;
		physicsComponentType["friction"] = &PhysicsComponent::friction_;
		physicsComponentType["restitution"] = &PhysicsComponent::restitution_;
		physicsComponentType["centerOfMassOffset"] =
			&PhysicsComponent::centerOfMassOffset_;
		physicsComponentType["velocity"] = &PhysicsComponent::velocity_;
		physicsComponentType["angularVelocity"] =
			&PhysicsComponent::angularVelocity_;
		physicsComponentType["colliderParams"] =
			&PhysicsComponent::colliderParams_;
	}

	static void BindPhysicsCharacterComponent(sol::state& state)
	{
		auto physicsCharacter = state.new_usertype<PhysicsCharacterComponent>(
			"PhysicsCharacterComponent");
		physicsCharacter["max_slope_angle"] =
			&PhysicsCharacterComponent::maxSlopeAngle_;
		physicsCharacter["control_movement_during_jump_"] =
			&PhysicsCharacterComponent::controlMovementDuringJump_;
		physicsCharacter["yaw"] = &PhysicsCharacterComponent::yaw_;
		physicsCharacter["pitch"] = &PhysicsCharacterComponent::pitch_;
		physicsCharacter["speed"] = &PhysicsCharacterComponent::speed_;
		physicsCharacter["jump_speed"] = &PhysicsCharacterComponent::jumpSpeed_;
		physicsCharacter["sensitivity_x"] =
			&PhysicsCharacterComponent::sensitivityX_;
		physicsCharacter["sensitivity_y"] =
			&PhysicsCharacterComponent::sensitivityY_;
		physicsCharacter["mass"] = &PhysicsCharacterComponent::mass_;
		physicsCharacter["friction"] = &PhysicsCharacterComponent::friction_;
		physicsCharacter["restitution"] =
			&PhysicsCharacterComponent::restitution_;
		physicsCharacter["velocity"] = &PhysicsCharacterComponent::velocity_;
		physicsCharacter["angularVelocity"] =
			&PhysicsCharacterComponent::angularVelocity_;
		physicsCharacter["colliderParams"] =
			&PhysicsCharacterComponent::colliderParams_;
	}

	static void BindInfoComponent(sol::state& state)
	{
		auto infoComponent =
			state.new_usertype<InfoAdapter>("ObjectInfoComponent");
		infoComponent["tag"] = &InfoAdapter::tag;
		infoComponent["name"] = &InfoAdapter::name;
	}

	static void BindBodyID(sol::state& state)
	{
		auto bodyId = state.new_usertype<JPH::BodyID>("BodyID");
	}

	static void BindBody(sol::state& state)
	{
		auto body = state.new_usertype<JPH::Body>("BodyID");
	}

	static void BindECSAdapter(sol::state& state)
	{
		auto adapter = state.new_usertype<LuaECSAdapter>("Component");
		adapter["get_component"] = &LuaECSAdapter::GetComponent;
		adapter["get_script_component"] = &LuaECSAdapter::GetScriptComponent;
		adapter["find_with_body_id"] = &LuaECSAdapter::FindWithBodyId;
		adapter["find_with_character_body_id"] =
			&LuaECSAdapter::FindWithCharacterBodyId;
		adapter["find_with_tag"] = &LuaECSAdapter::FindWithTag;
		adapter["find_all_with_tag"] = &LuaECSAdapter::FindAllWithTag;
		adapter["destroy"] = &LuaECSAdapter::Destroy;
		adapter["destroy_many"] = &LuaECSAdapter::DestroyMany;
		adapter["self_destroy"] = &LuaECSAdapter::SelfDestroy;
		adapter["load_world"] = &ECSUtils::LoadWorld;
	}

	static void BindInputAdapter(sol::state& state)
	{
		auto adapter = state.new_usertype<InputDeviceAdapter>("Input");
		adapter["is_key_down"] = &InputDeviceAdapter::IsKeyDown;
	}

	static void BindPhysicsAdapter(sol::state& state)
	{
		auto adapter = state.new_usertype<PhysicsActivationAdapter>(
			"Physics", sol::no_constructor);
		adapter["set_active"] = &PhysicsActivationAdapter::SetActive;
		adapter["set_trigger"] = &PhysicsActivationAdapter::SetTrigger;
	}

	static void BindRender(sol::state& state)
	{
		auto render =
			state.new_usertype<RenderUtils>("Render", sol::no_constructor);
		render["set_tonemapper_exposure"] = &RenderUtils::SetTonemapperExposure;
		render["get_tonemapper_exposure"] = &RenderUtils::GetTonemapperExposure;
	}

	static void BindDebug(sol::state& state)
	{
		struct Logger
		{
		};

		auto logger = state.new_usertype<Logger>("Log", sol::no_constructor);
		logger["message"] = [](const std::string& msg)
		{ Debug::LogMessage(msg.c_str()); };
		logger["warning"] = [](const std::string& msg)
		{ Debug::LogWarning(msg.c_str()); };
		logger["error"] = [](const std::string& msg)
		{ Debug::LogError(msg.c_str()); };
		logger["assert"] = [](bool condition, const std::string& msg)
		{ Debug::Assert(condition, msg.c_str()); };
	}

	static void BindLight(sol::state& state)
	{
		auto light =
			state.new_usertype<LightComponent>("Light", sol::no_constructor);
		light["intensity"] = &LightComponent::intensity;
	}

	static void BindAction(sol::state& state)
	{
		auto action = state.new_usertype<Action>(
			"Action", sol::constructors<Action(const std::string&)>());
		action["set_cost"] = &Action::SetCost;
		action["add_precondition"] = &Action::AddPrecondition;
		action["add_effect"] = &Action::AddEffect;
	}

	static void BindGoal(sol::state& state)
	{
		auto goal = state.new_usertype<Goal>(
			"Goal", sol::constructors<Goal(const std::string&, int)>());
		goal["add_precondition"] = &Goal::AddPrecondition;
		goal["add_target"] = &Goal::AddTarget;
	}

	static void BindAiAgentAdapter(sol::state& state)
	{
		auto agent =
			state.new_usertype<AiAgentAdapter>("Agent", sol::no_constructor);
		agent["add_goal"] = &AiAgentAdapter::AddGoal;
		agent["add_action"] = sol::overload(
			sol::resolve<const Action&, sol::function>(
				&AiAgentAdapter::AddAction),
			sol::resolve<void(const Action&, sol::table, sol::function)>(
				&AiAgentAdapter::AddAction),
			sol::resolve<const Action&, sol::table, sol::function,
		                 sol::function>(&AiAgentAdapter::AddAction));
		agent["set_name"] = &AiAgentAdapter::SetName;
		agent["set_state_fact"] = &AiAgentAdapter::SetStateFact;
	}

	static void BindTime(sol::state& state)
	{
		struct CustomTime
		{
		};
		auto time = state.new_usertype<CustomTime>("Time", sol::no_constructor);
		time["delta_time"] = &Time::DeltaTime;
	}

	static void BindNavigationManager(sol::state& state)
	{
		struct Dummy
		{
		};
		auto time =
			state.new_usertype<Dummy>("Navigation", sol::no_constructor);
		time["move_to"] = &NavigationManager::MoveTo;
		time["cancel_target"] = &NavigationManager::CancelTarget;
	}

	static void BindEngineTypes(sol::state& state)
	{
		BindComponentType(state);
		BindVector(state);
		BindMatrix(state);
		BindQuaternion(state);
		BindTransformCompoent(state);
		BindPhysicsComponent(state);
		BindPhysicsCharacterComponent(state);
		BindInfoComponent(state);
		BindBodyID(state);
		BindBody(state);
		BindECSAdapter(state);
		BindInputAdapter(state);
		BindPhysicsAdapter(state);
		BindRender(state);
		BindDebug(state);
		BindAction(state);
		BindGoal(state);
		BindAiAgentAdapter(state);
		BindTime(state);
		BindNavigationManager(state);
		BindLight(state);
	}
} // namespace D3E