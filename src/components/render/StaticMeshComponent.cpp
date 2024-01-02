#include "D3E/Components/render/StaticMeshComponent.h"
#include "json.hpp"

namespace D3E
{
	void to_json(json& j, const StaticMeshComponent& t)
	{
		j = json{{"type", "component"},
		         {"class", "StaticMeshComponent"},
		         {"mesh_uuid", t.meshUuid.c_str()},
		         {"pipeline", t.pipelineName.c_str()},
		         {"material_uuid", t.materialUuid.c_str()}
		};
	}

	void from_json(const json& j, StaticMeshComponent& t)
	{
		std::string mesh_uuid;
		std::string pipeline;
		std::string material_uuid;

		j.at("mesh_uuid").get_to(mesh_uuid);
		j.at("pipeline").get_to(pipeline);
		j.at("material_uuid").get_to(material_uuid);

		t.meshUuid = mesh_uuid.c_str();
		t.pipelineName = pipeline.c_str();
		t.materialUuid = material_uuid.c_str();
	}
}
