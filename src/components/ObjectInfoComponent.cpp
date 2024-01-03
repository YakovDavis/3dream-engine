#include "D3E/Components/ObjectInfoComponent.h"
#include "json.hpp"

namespace D3E
{
	void to_json(json& j, const ObjectInfoComponent& t)
	{
		j = json{{"type", "component"},
		         {"class", "ObjectInfoComponent"},
		         {"name", t.name.c_str()},
		         {"id", t.id.c_str()},
		         {"visible", t.visible}
		};
	}

	void from_json(const json& j, ObjectInfoComponent& t)
	{
		std::string id;
		std::string name;

		j.at("id").get_to(id);
		j.at("name").get_to(name);
		j.at("visible").get_to(t.visible);

		t.id = id.c_str();
		t.name = name.c_str();
	}
}

