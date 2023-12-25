#pragma once

#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Components/TransformComponent.h"
#include "SimpleMath.h"
#include "sol/sol.hpp"

namespace D3E
{
	void BindEngineTypes(sol::state& state)
	{
		auto transformType =
			state.new_usertype<TransformComponent>("Transform");
		transformType["position"] = &D3E::TransformComponent::position;
		transformType["rotation"] = &D3E::TransformComponent::rotation;
		transformType["scale"] = &D3E::TransformComponent::scale;

		auto vector3Type =
			state.new_usertype<DirectX::SimpleMath::Vector3>("Vector3");
		vector3Type["x"] = &DirectX::SimpleMath::Vector3::x;
		vector3Type["y"] = &DirectX::SimpleMath::Vector3::y;
		vector3Type["z"] = &DirectX::SimpleMath::Vector3::z;

		auto quaternionType =
			state.new_usertype<DirectX::SimpleMath::Quaternion>("Quaternion");
		quaternionType["x"] = &DirectX::SimpleMath::Quaternion::x;
		quaternionType["y"] = &DirectX::SimpleMath::Quaternion::y;
		quaternionType["z"] = &DirectX::SimpleMath::Quaternion::z;
		quaternionType["w"] = &DirectX::SimpleMath::Quaternion::w;
	}
} // namespace D3E