#pragma once

#include "D3E/CommonHeader.h"

namespace D3E
{
	struct Material
	{
		String albedoTextureUuid;
		String normalTextureUuid;
		String metalnessTextureUuid;
		String roughnessTextureUuid;
		String specularTextureUuid;
		String irradianceTextureUuid;
		String specularBrdfLutUuid;
	};
}
