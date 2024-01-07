#pragma once

#include "D3E/CommonHeader.h"
#include "D3E/Components/BaseComponent.h"

namespace D3E
{
	struct SoundListenerComponent : public BaseComponent
	{
		void to_json(json& j) const override;
		void from_json(const json& j) override;
	};

	void to_json(json& j, const SoundListenerComponent& t);

	void from_json(const json& j, SoundListenerComponent& t);
} // namespace D3E