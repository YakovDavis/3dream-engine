#pragma once

#include "D3E/CommonHeader.h"
#include "../../../libs/nlohmann/json.hpp"

using json = nlohmann::json;

namespace D3E
{
	enum MaterialType
	{
		Lit
	};

	struct D3EAPI Material
	{
		String uuid;
		String name;

		MaterialType type;

		String albedoTextureUuid;
		String normalTextureUuid;
		String metalnessTextureUuid;
		String roughnessTextureUuid;
	};

	void to_json(json& j, const Material& t);

	void from_json(const json& j, Material& t);
}
