#pragma once

#include "BaseComponent.h"
#include "D3E/Debug.h"
#include "EASTL/string.h"
#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/Body.h"
#include "entt/entt.hpp"
#include "sol/sol.hpp"

namespace D3E
{
	struct ScriptComponent : public BaseComponent
	{
		friend class ScriptingEngine;

		explicit ScriptComponent(entt::entity ownerId) : ownerId_{ownerId} {}

		void Init() { ValidateCallResult(init(self)); }

		void Start() { ValidateCallResult(start(self)); }

		void Update(float deltaTime)
		{
			ValidateCallResult(update(self, deltaTime));
		}

		void OnCollisionEnter(const JPH::BodyID& bodyId)
		{
			ValidateCallResult(onCollisionEnter(self, bodyId));
		}

		void OnCollisionStay(const JPH::BodyID& bodyId)
		{
			ValidateCallResult(onCollisionStay(self, bodyId));
		}

		void OnCollisionExit(const JPH::BodyID& bodyId)
		{
			ValidateCallResult(onCollisionExit(self, bodyId));
		}

		entt::entity GetOwnerId() const { return ownerId_; }

		const String& GetEntryPoint() const { return entryPoint_; }
		void SetEntryPoint(String& entryPoint) { entryPoint_ = entryPoint; }

		void to_json(json& j) const override;
		void from_json(const json& j) override;

	private:
		String entryPoint_;
		entt::entity ownerId_;
		sol::table self;
		sol::function init;
		sol::function start;
		sol::function update;
		sol::function onCollisionEnter;
		sol::function onCollisionStay;
		sol::function onCollisionExit;

		void ValidateCallResult(const sol::protected_function_result& r)
		{
			if (!r.valid())
			{
				sol::error err = r;
				std::string what = err.what();

				Debug::LogError(what.c_str());
			}
		}
	};

	void to_json(json& j, const ScriptComponent& sc);
	void from_json(const json& j, ScriptComponent& sc);
} // namespace D3E