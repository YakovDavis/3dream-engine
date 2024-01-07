#include "D3E/Components/sound/SoundComponent.h"
#include "json.hpp"

namespace D3E
{
	void to_json(json& j, const SoundComponent& t)
	{
		t.to_json(j);
	}

	void from_json(const json& j, SoundComponent& t)
	{
		t.from_json(j);
	}

	void SoundComponent::to_json(json& j) const
	{
		j = json{{"type", "component"},
		         {"class", "SoundComponent"},
		         {"is_3D", is3D},
		         {"is_looping", isLooping},
		         {"is_streaming", isStreaming},
		         {"volume", volume},
		         {"location", std::vector({location.x, location.y, location.z})},
		         {"file_name", fileName.c_str()}
		};
	}

	void SoundComponent::from_json(const json& j)
	{
		std::vector<float> tmp_location(3);
		std::string tmp_fileName;

		j.at("is_3D").get_to(is3D);
		j.at("is_looping").get_to(isLooping);
		j.at("is_streaming").get_to(isStreaming);
		j.at("volume").get_to(volume);
		j.at("location").get_to(tmp_location);
		j.at("file_name").get_to(tmp_fileName);

		location = DirectX::SimpleMath::Vector3(tmp_location[0], tmp_location[1], tmp_location[2]);
		fileName = tmp_fileName.c_str();
	}
}
