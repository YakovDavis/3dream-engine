#include "D3E/Components/render/CameraComponent.h"
#include "render/CameraUtils.h"
#include "json.hpp"

using namespace DirectX::SimpleMath;

namespace D3E
{
	void to_json(json& j, const CameraComponent& t)
	{
		t.to_json(j);
	}

	void from_json(const json& j, CameraComponent& t)
	{
		t.from_json(j);
	}

	void CameraComponent::to_json(json& j) const
	{
		j = json{{"type", "component"},
		         {"class", "CameraComponent"},
		         {"offset", std::vector({offset.x, offset.y, offset.z})},
		         {"initial_offset", std::vector({initialOffset.x, initialOffset.y, initialOffset.z})},
		         {"forward", std::vector({forward.x, forward.y, forward.z})},
		         {"up", std::vector({up.x, up.y, up.z})},
		         {"is_orthographic", isOrthographic},
		         {"orthographic_width", orthographicWidth},
		         {"orthographic_height", orthographicHeight},
		         {"fov", fov},
		         {"aspect_ratio", aspectRatio},
		         {"near_plane", nearPlane},
		         {"far_plane", farPlane}
		};
	}

	void CameraComponent::from_json(const json& j)
	{
		std::vector<float> tmp_offset(3);
		std::vector<float> tmp_initialOffset(3);
		std::vector<float> tmp_forward(3);
		std::vector<float> tmp_up(3);

		j.at("offset").get_to(tmp_offset);
		j.at("initial_offset").get_to(tmp_initialOffset);
		j.at("forward").get_to(tmp_forward);
		j.at("up").get_to(tmp_up);
		j.at("is_orthographic").get_to(isOrthographic);
		j.at("orthographic_width").get_to(orthographicWidth);
		j.at("orthographic_height").get_to(orthographicHeight);
		j.at("fov").get_to(fov);
		j.at("aspect_ratio").get_to(aspectRatio);
		j.at("near_plane").get_to(nearPlane);
		j.at("far_plane").get_to(farPlane);

		offset = Vector3(tmp_offset[0], tmp_offset[1], tmp_offset[2]);
		initialOffset = Vector3(tmp_initialOffset[0], tmp_initialOffset[1], tmp_initialOffset[2]);
		forward = Vector3(tmp_forward[0], tmp_forward[1], tmp_forward[2]);
		up = Vector3(tmp_up[0], tmp_up[1], tmp_up[2]);
	}

	CameraComponent::CameraComponent()
	{
		boundingFrustum = DirectX::BoundingFrustum((DirectX::CXMMATRIX)CameraUtils::GetProj(*this), false);
	}
}
