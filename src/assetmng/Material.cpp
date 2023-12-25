#include "D3E/render/Material.h"

#include "core/magic_enum.hpp"

void D3E::to_json(json& j, const Material& t)
{
	j = json{{"type", "material"},
	         {"uuid", t.uuid.c_str()},
	         {"name", t.name.c_str()},
	         {"material_type", magic_enum::enum_name(t.type)},
	         {"textures", {
							{"albedo_texture", t.albedoTextureUuid.c_str()},
							{"normal_texture", t.normalTextureUuid.c_str()},
							{"metalness_texture", t.metalnessTextureUuid.c_str()},
							{"roughness_texture", t.roughnessTextureUuid.c_str()}
						}
	         }
	};
}

void D3E::from_json(const json& j, Material& t)
{
	std::string uuid;
	std::string name;
	std::string material_type;
	std::string albedo_texture;
	std::string normal_texture;
	std::string metalness_texture;
	std::string roughness_texture;

	j.at("uuid").get_to(uuid);
	j.at("name").get_to(name);
	j.at("material_type").get_to(material_type);
	j.at("textures").at("albedo_texture").get_to(albedo_texture);
	j.at("textures").at("normal_texture").get_to(normal_texture);
	j.at("textures").at("metalness_texture").get_to(metalness_texture);
	j.at("textures").at("roughness_texture").get_to(roughness_texture);

	t.uuid = uuid.c_str();
	t.name = name.c_str();
	t.albedoTextureUuid = albedo_texture.c_str();
	t.normalTextureUuid = normal_texture.c_str();
	t.metalnessTextureUuid = metalness_texture.c_str();
	t.roughnessTextureUuid = roughness_texture.c_str();

	auto c = magic_enum::enum_cast<MaterialType>(material_type);
	if (c.has_value())
	{
		t.type = c.value();
	}
}
