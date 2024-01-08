#include "D3E/Components/ObjectInfoComponent.h"

#include "editor/EditorIdManager.h"
#include "json.hpp"

namespace D3E
{
	void to_json(json& j, const ObjectInfoComponent& t)
	{
		t.to_json(j);
	}

	void from_json(const json& j, ObjectInfoComponent& t)
	{
		t.from_json(j);
	}

	void ObjectInfoComponent::to_json(json& j) const
	{
		j = json{{"type", "component"},
		         {"class", "ObjectInfoComponent"},
		         {"name", name.c_str()},
		         {"id", id.c_str()},
		         {"visible", visible},
		         {"serialize_entity", serializeEntity}
		};
	}

	void ObjectInfoComponent::from_json(const json& j)
	{
		std::string tmp_id;
		std::string tmp_name;

		j.at("id").get_to(tmp_id);
		j.at("name").get_to(tmp_name);
		j.at("visible").get_to(visible);
		j.at("serialize_entity").get_to(serializeEntity);

		if (tmp_id == EmptyIdStdStr)
		{
			tmp_id = UuidGenerator::NewGuidStdStr();
		}

		id = tmp_id.c_str();
		name = tmp_name.c_str();
		editorId = EditorIdManager::Get()->RegisterUuid(id);
	}
}

