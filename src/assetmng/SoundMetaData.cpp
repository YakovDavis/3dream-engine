#include "SoundMetaData.h"

void D3E::to_json(json& j, const D3E::SoundMetaData& t)
{
	j = json{{"type", "sound"},
	         {"uuid", t.uuid},
	         {"name", t.name},
	         {"filename", t.filename},
	         {"is_3d", t.is3d},
	         {"is_looping", t.isLooping},
	         {"stream", t.stream}
	};
}

void D3E::from_json(const json& j, D3E::SoundMetaData& t)
{
	j.at("uuid").get_to(t.uuid);
	j.at("name").get_to(t.name);
	j.at("filename").get_to(t.filename);
	j.at("is_3d").get_to(t.is3d);
	j.at("is_looping").get_to(t.isLooping);
	j.at("stream").get_to(t.stream);
}
