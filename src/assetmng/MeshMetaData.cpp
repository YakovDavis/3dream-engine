#include "MeshMetaData.h"

namespace D3E
{
	void to_json(json& j, const MeshMetaData& t)
	{
		j = json{{"type", "mesh"},
		         {"uuid", t.uuid},
		         {"name", t.name},
		         {"filename", t.filename},
				 {"input_layout", t.inputLayout},
		};
	}

	void from_json(const json& j, MeshMetaData& t)
	{
		std::string channels;

		j.at("uuid").get_to(t.uuid);
		j.at("name").get_to(t.name);
		j.at("filename").get_to(t.filename);

		if (j.contains("input_layout"))
		{
			j.at("input_layout").get_to(t.inputLayout);
		}
		else
		{
			t.inputLayout = "default";
		}
	}
}
