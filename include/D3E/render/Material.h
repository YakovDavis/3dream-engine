#pragma once

#include "D3E/CommonHeader.h"

namespace D3E
{
	struct Material
	{
		String uuid;

		String albedoTextureUuid;
		String normalTextureUuid;
		String metalnessTextureUuid;
		String roughnessTextureUuid;
	};
}
