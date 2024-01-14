#pragma once

#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Components/PhysicsCharacterComponent.h"
#include "D3E/Components/PhysicsComponent.h"
#include "D3E/Components/TransformComponent.h"
#include "Jolt/Physics/Body/Body.h"
#include "SimpleMath.h"
#include "scripting/LuaECSAdapter.h"
#include "sol/sol.hpp"
#include "utils/ECSUtils.h"

namespace D3E
{
	using namespace DirectX::SimpleMath;

	static void BindComponentType(sol::state& state)
	{
		state["ComponentType"] = state.create_table_with(
			"Transform", ComponentType::kTransformComponent, "Script",
			ComponentType::kScriptComponent, "Physics",
			ComponentType::kPhysicsComponent);
	}

	static void BindMatrix(sol::state& state)
	{
		auto matrixType = state.new_usertype<Matrix>("Martix");

		matrixType["create_from_quaternion"] = &Matrix::CreateFromQuaternion;
		matrixType["create_from_yaw_pith_roll"] = sol::overload(
			sol::resolve<float, float, float>(Matrix::CreateFromYawPitchRoll),
			sol::resolve<const Vector3&>(Matrix::CreateFromYawPitchRoll));
	}

	static void BindVector(sol::state& state)
	{
		auto vector3Type = state.new_usertype<Vector3>(
			"Vector3",
			sol::constructors<Vector3(), Vector3(float, float, float)>(),
			sol::meta_function::addition,
			sol::resolve<const Vector3&, const Vector3&>(::operator+),
			sol::meta_function::subtraction, &Vector3::operator-);

		vector3Type["x"] = &Vector3::x;
		vector3Type["y"] = &Vector3::y;
		vector3Type["z"] = &Vector3::z;
		vector3Type["transform"] =
			sol::resolve<Vector3(const Vector3&, const Matrix&)>(
				Vector3::Transform);
	}

	static void BindQuaternion(sol::state& state)
	{
		auto quaternionType = state.new_usertype<Quaternion>(
			"Quaternion",
			sol::constructors<Quaternion(),
		                      Quaternion(float, float, float, float)>());

		quaternionType["x"] = &Quaternion::x;
		quaternionType["y"] = &Quaternion::y;
		quaternionType["z"] = &Quaternion::z;
		quaternionType["w"] = &Quaternion::w;
		quaternionType["to_euler"] = &Quaternion::ToEuler;

		quaternionType["create_from_yaw_pitch_roll"] = sol::overload(
			sol::resolve<float, float, float>(
				Quaternion::CreateFromYawPitchRoll),
			sol::resolve<const Vector3&>(Quaternion::CreateFromYawPitchRoll));
	}

	static void BindTransformCompoent(sol::state& state)
	{
		auto transformComponentType =
			state.new_usertype<TransformComponent>("TransformComponent");

		transformComponentType["position"] = &D3E::TransformComponent::position;
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
		BindBodyID(state);
		BindBody(state);
		BindECSAdapter(state);
	}
} // namespace D3E