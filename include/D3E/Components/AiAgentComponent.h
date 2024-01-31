#pragma once

#include "D3E/Components/BaseComponent.h"
#include "D3E/ai/FSM.h"
#include "D3E/ai/Agent.h"

namespace D3E
{
	struct D3EAPI AiAgentComponent : public BaseComponent
	{
		Agent agent;
		FSM fsm;
		FSMState idle;
		FSMState moveTo;
		FSMState perform;

		AiAgentComponent();

		void CreateIdle();
		void CreateMoveTo();
		void CreatePerform();

		void to_json(json& j) const override;
		void from_json(const json& j) override;

		void Update() const;
	};

	void to_json(json& j, const AiAgentComponent& t);

	void from_json(const json& j, AiAgentComponent& t);
} // namespace D3E