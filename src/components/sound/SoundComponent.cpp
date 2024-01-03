#include "D3E/Components/sound/SoundComponent.h"
#include "json.hpp"

namespace D3E
{
	void to_json(json& j, const SoundComponent& t)
	{
		j = json{{"type", "component"},
		         {"class", "SoundComponent"},
		         {"is_3D", t.is3D},
		         {"is_looping", t.isLooping},
		         {"is_streaming", t.isStreaming},
		         {"volume", t.volume},
			     {"location", std::vector({t.location.x, t.location.y, t.location.z})},
		         {"file_name", t.fileName.c_str()}
		};
	}

	void from_json(const json& j, SoundComponent& t)
	{
		std::vector<float> location(3);
		std::string file_name;

		j.at("is_3D").get_to(t.is3D);
		j.at("is_looping").get_to(t.isLooping);
		j.at("is_streaming").get_to(t.isStreaming);
		j.at("volume").get_to(t.volume);
		j.at("location").get_to(location);
		j.at("file_name").get_to(file_name);

		t.location = DirectX::SimpleMath::Vector3(location[0], location[1], location[2]);
		t.fileName = file_name.c_str();
	}
}
