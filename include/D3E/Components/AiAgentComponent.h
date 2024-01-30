#pragma once

#include "D3E/Components/BaseComponent.h"
#include "D3E/ai/FSM.h"

namespace D3E
{
	struct D3EAPI AiAgentComponent : public BaseComponent
	{
		FSM fsm;
		void to_json(json& j) const override;
		void from_json(const json& j) override;
	};

	void to_json(json& j, const AiAgentComponent& t);

	void from_json(const json& j, AiAgentComponent& t);
} // namespace D3E