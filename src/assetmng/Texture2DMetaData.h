#pragma once

#include "D3E/CommonHeader.h"
#include "EASTL/fixed_vector.h"
#include "EASTL/string.h"
#include "json.hpp"

using json = nlohmann::json;

namespace D3E
{
	enum TextureChannels
	{
		RGBA8
	};

	struct Texture2DMetaData
	{
		std::string uuid;
		std::string name;
		std::string filename;
		struct Texture2DFormat
		{
			TextureChannels channels = TextureChannels::RGBA8;
			eastl::fixed_vector<uint32_t, 2> dimensions = {1024, 1024};
		};
		Texture2DFormat format;
	};

	void to_json(json& j, const Texture2DMetaData& t);

	void from_json(const json& j, Texture2DMetaData& t);
}
