#pragma once

#include "D3E/CommonHeader.h"

namespace D3E
{
	struct BaseComponent
	{
		virtual void to_json(json& j) const = 0;
		virtual void from_json(const json& j) = 0;
	};
}
