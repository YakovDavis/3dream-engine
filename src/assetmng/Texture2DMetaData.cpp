#include "Texture2DMetaData.h"

#include "core/magic_enum.hpp"

namespace D3E
{
	void to_json(json& j, const Texture2DMetaData& t)
	{
		j = json{{"type", "texture2d"},
		         {"uuid", t.uuid},
		         {"name", t.name},
		         {"filename", t.filename},
		         {"lock_resource_state", t.lockResourceState},
		         {"format", {
								{"channels", magic_enum::enum_name(t.format.channels)},
								{"type", magic_enum::enum_name(t.format.type)},
								{"dimensions", std::vector({t.format.dimensions[0], t.format.dimensions[1]})}
							}
		         }
		};
	}

	void from_json(const json& j, Texture2DMetaData& t)
	{
		std::string channels;
		std::string type;

		j.at("uuid").get_to(t.uuid);
		j.at("name").get_to(t.name);
		j.at("filename").get_to(t.filename);
		j.at("format").at("channels").get_to(channels);
		j.at("format").at("type").get_to(type);
		j.at("format").at("dimensions")[0].get_to(t.format.dimensions[0]);
		j.at("format").at("dimensions")[1].get_to(t.format.dimensions[1]);

		if (j.contains("lock_resource_state"))
		{
			j.at("lock_resource_state").get_to(t.lockResourceState);
		}
		else
		{
			t.lockResourceState = true;
		}

		auto c = magic_enum::enum_cast<TextureChannels>(channels);
		if (c.has_value())
		{
			t.format.channels = c.value();
		}
		else
		{
			t.format.channels = TextureChannels::RGBA8;
		}

		auto t1 = magic_enum::enum_cast<TextureType>(type);
		if (t1.has_value())
		{
			t.format.type = t1.value();
		}
		else
		{
			t.format.type = TextureType::Texture2D;
		}
	}
}
