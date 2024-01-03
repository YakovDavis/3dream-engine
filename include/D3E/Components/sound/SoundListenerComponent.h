#pragma once

#include "D3E/CommonHeader.h"

namespace D3E
{
	struct SoundListenerComponent
	{
	};

	void to_json(json& j, const SoundListenerComponent& t);

	void from_json(const json& j, SoundListenerComponent& t);
} // namespace D3E