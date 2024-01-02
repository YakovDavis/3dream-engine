#include "D3E/Components/render/CameraComponent.h"
#include "json.hpp"

using namespace DirectX::SimpleMath;

namespace D3E
{
	void to_json(json& j, const CameraComponent& t)
	{
		j = json{{"type", "component"},
			{"class", "CameraComponent"},
			{"offset", std::vector({t.offset.x, t.offset.y, t.offset.z})},
			{"initial_offset", std::vector({t.initialOffset.x, t.initialOffset.y, t.initialOffset.z})},
			{"forward", std::vector({t.forward.x, t.forward.y, t.forward.z})},
			{"up", std::vector({t.up.x, t.up.y, t.up.z})},
			{"is_orthographic", t.isOrthographic},
			{"orthographic_width", t.orthographicWidth},
			{"orthographic_height", t.orthographicHeight},
			{"fov", t.fov},
			{"aspect_ratio", t.aspectRatio},
			{"near_plane", t.nearPlane},
			{"far_plane", t.farPlane}
		};
	}

	void from_json(const json& j, CameraComponent& t)
	{
		std::vector<float> offset(3);
		std::vector<float> initialOffset(3);
		std::vector<float> forward(3);
		std::vector<float> up(3);

		j.at("offset").get_to(offset);
		j.at("initial_offset").get_to(initialOffset);
		j.at("forward").get_to(forward);
		j.at("up").get_to(up);
		j.at("is_orthographic").get_to(t.isOrthographic);
		j.at("orthographic_width").get_to(t.orthographicWidth);
		j.at("orthographic_height").get_to(t.orthographicHeight);
		j.at("fov").get_to(t.fov);
		j.at("aspect_ratio").get_to(t.aspectRatio);
		j.at("near_plane").get_to(t.nearPlane);
		j.at("far_plane").get_to(t.farPlane);

		t.offset = Vector3(offset[0], offset[1], offset[2]);
		t.initialOffset = Vector3(initialOffset[0], initialOffset[1], initialOffset[2]);
		t.forward = Vector3(forward[0], forward[1], forward[2]);
		t.up = Vector3(up[0], up[1], up[2]);
	}
}
