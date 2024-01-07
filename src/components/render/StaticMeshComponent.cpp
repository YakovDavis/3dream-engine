#include "D3E/Components/render/StaticMeshComponent.h"
#include "json.hpp"

namespace D3E
{
	void to_json(json& j, const StaticMeshComponent& t)
	{
		t.to_json(j);
	}

	void from_json(const json& j, StaticMeshComponent& t)
	{
		t.from_json(j);
	}

	void StaticMeshComponent::to_json(json& j) const
	{
		j = json{{"type", "component"},
		         {"class", "StaticMeshComponent"},
		         {"mesh_uuid", meshUuid.c_str()},
		         {"pipeline", pipelineName.c_str()},
		         {"material_uuid", materialUuid.c_str()}
		};
	}

	void StaticMeshComponent::from_json(const json& j)
	{
		std::string tmp_meshUuid;
		std::string tmp_pipelineName;
		std::string tmp_materialUuid;

		j.at("mesh_uuid").get_to(tmp_meshUuid);
		j.at("pipeline").get_to(tmp_pipelineName);
		j.at("material_uuid").get_to(tmp_materialUuid);

		meshUuid = tmp_meshUuid.c_str();
		pipelineName = tmp_pipelineName.c_str();
		materialUuid = tmp_materialUuid.c_str();
	}
}
