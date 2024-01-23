#pragma once

#include "BaseComponent.h"
#include "D3E/Debug.h"
#include "D3E/Uuid.h"
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

		explicit ScriptComponent(entt::entity ownerId)
			: ScriptComponent(ownerId, EmptyIdString)
		{
		}

		ScriptComponent(entt::entity ownerId, const String& scriptUuid)
			: ownerId_(ownerId), scriptUuid_(scriptUuid), loaded_(false)
		{
		}

		void Init()
		{
			if (!loaded_)
			{
				return;
			}

			ValidateCallResult(init_(self_));
		}

		void Start()
		{
			if (!loaded_)
			{
				return;
			}

			ValidateCallResult(start_(self_));
		}

		void Update(float deltaTime)
		{
			if (!loaded_)
			{
				return;
			}

			ValidateCallResult(update_(self_, deltaTime));
		}

		void OnCollisionEnter(const JPH::BodyID& bodyId)
		{
			if (!loaded_)
			{
				return;
			}

			ValidateCallResult(onCollisionEnter_(self_, bodyId));
		}

		void OnCollisionStay(const JPH::BodyID& bodyId)
		{
			if (!loaded_)
			{
				return;
			}

			ValidateCallResult(onCollisionStay_(self_, bodyId));
		}

		void OnCollisionExit(const JPH::BodyID& bodyId)
		{
			if (!loaded_)
			{
				return;
			}

			ValidateCallResult(onCollisionExit_(self_, bodyId));
		}

		void OnClicked(entt::entity e)
		{
			if (!loaded_)
			{
				return;
			}

			ValidateCallResult(onClicked_(self_, e));
		}

		entt::entity GetOwnerId() const { return ownerId_; }
		sol::table& GetSelf() { return self_; }
		const String& GetEntryPoint() const { return entryPoint_; }
		const String& GetScriptUuid() const { return scriptUuid_; }
		void SetScriptUuid(const String& uuid) { scriptUuid_ = uuid; }
		void SetEntryPoint(String& entryPoint) { entryPoint_ = entryPoint; }
		void Free()
		{
			self_ = sol::nil;
			init_ = sol::nil;
			start_ = sol::nil;
			update_ = sol::nil;
			onCollisionEnter_ = sol::nil;
			onCollisionStay_ = sol::nil;
			onCollisionExit_ = sol::nil;
			onClicked_ = sol::nil;
		}

		void to_json(json& j) const override;
		void from_json(const json& j) override;

	private:
		bool loaded_;
		String scriptUuid_;
		String entryPoint_;
		entt::entity ownerId_;
		sol::table self_;
		sol::function init_;
		sol::function start_;
		sol::function update_;
		sol::function onCollisionEnter_;
		sol::function onCollisionStay_;
		sol::function onCollisionExit_;
		sol::function onClicked_;

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