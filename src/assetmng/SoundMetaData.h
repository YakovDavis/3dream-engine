#pragma once

#include "D3E/CommonHeader.h"
#include "EASTL/fixed_vector.h"
#include "json.hpp"

using json = nlohmann::json;

namespace D3E
{
	struct SoundMetaData
	{
		std::string uuid;
		std::string name;
		std::string filename;
		bool is3d = false;
		bool isLooping = false;
		bool stream = false;
	};

	void to_json(json& j, const SoundMetaData& t);

	void from_json(const json& j, SoundMetaData& t);
}
