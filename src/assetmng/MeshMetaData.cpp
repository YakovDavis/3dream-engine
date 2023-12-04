#include "MeshMetaData.h"

namespace D3E
{
	void to_json(json& j, const MeshMetaData& t)
	{
		j = json{{"type", "mesh"},
		         {"uuid", t.uuid},
		         {"name", t.name},
		         {"filename", t.filename}
		};
	}

	void from_json(const json& j, MeshMetaData& t)
	{
		std::string channels;

		j.at("uuid").get_to(t.uuid);
		j.at("name").get_to(t.name);
		j.at("filename").get_to(t.filename);
	}
}
