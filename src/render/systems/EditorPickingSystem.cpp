#include "EditorPickingSystem.h"

#include "D3E/Components/MouseComponent.h"
#include "D3E/Components/ObjectInfoComponent.h"
#include "D3E/Components/TransformComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Components/render/StaticMeshComponent.h"
#include "D3E/Debug.h"
#include "SimpleMath.h"
#include "assetmng/MeshFactory.h"
#include "render/CameraUtils.h"

using namespace DirectX::SimpleMath;

void D3E::EditorPickingSystem::RunOnce(entt::registry& reg, D3E::Game* game,
                                       float dT)
{
	auto mouseView = reg.view<const MouseComponent>();

	const MouseComponent* mouse = nullptr;

	for(auto [entity, mc] : mouseView.each())
	{
		mouse = &mc;
		break;
	}

	if (!mouse)
	{
		return;
	}

	Vector3 origin = {0, 0, 0};

	auto playerView = reg.view<const TransformComponent, const CameraComponent>();

	const CameraComponent* camera = nullptr;

	for(auto [entity, tc, cc] : playerView.each())
	{
		origin = tc.position + cc.offset;
		camera = &cc;
		break;
	}

	if (!camera)
	{
		return;
	}

	Matrix p = CameraUtils::GetProj(*camera);

	float vx = (+2.0f * mouse->position.x / 1280.0f - 1.0f) / p(0, 0); // TODO: un-hardcode
	float vy = (+2.0f * mouse->position.y / 720.0f - 1.0f) / p(1, 1); // TODO: un-hardcode

	Vector4 rayOrigin(0.0f, 0.0f, 0.0f, 1.0f);
	Vector4 rayDir(vx, vy, 1.0f, 0.0f);

	Matrix v = CameraUtils::GetView(origin, *camera);
	Matrix invView = v.Invert();

	entt::entity picked = entt::null;

	auto view = reg.view<const ObjectInfoComponent, const TransformComponent, StaticMeshComponent>();

	view.each([&](const auto& info, const auto& tc, auto& smc)
	          {

				  const auto md = MeshFactory::GetMeshData(smc.meshUuid);

				  if(!info.visible)
				  {
					  return;
				  }

				  Matrix W = DirectX::SimpleMath::Matrix::CreateScale(tc.scale) * DirectX::SimpleMath::Matrix::CreateFromQuaternion(tc.rotation) * DirectX::SimpleMath::Matrix::CreateTranslation(tc.position);
				  Matrix invWorld = W.Invert();

				  Matrix toLocal = invView * invWorld;

				  rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
				  rayDir = XMVector3TransformNormal(rayDir, toLocal);

				  rayDir.Normalize();

				  // If we hit the bounding box of the Mesh, then we might have picked a Mesh triangle,
		          // so do the ray/triangle tests.
		          //
		          // If we did not hit the bounding box, then it is impossible that we hit
		          // the Mesh, so do not waste effort doing ray/triangle tests.
				  float tmin = 0.0f;
				  if(md.boundingBox.Intersects(rayOrigin, rayDir, tmin))
				  {
					  Debug::LogMessage(info.name);

					  // NOTE: For the demo, we know what to cast the vertex/index data to.  If we were mixing formats, some metadata would be needed to figure out what to cast it to.
					  UINT triCount = md.indices.size() / 3;

					  // Find the nearest ray/triangle intersection.
					  tmin = FLT_MAX;
					  for (UINT i = 0; i < triCount; ++i)
					  {
						  // Indices for this triangle.
						  UINT i0 = md.indices[i * 3 + 0];
						  UINT i1 = md.indices[i * 3 + 1];
						  UINT i2 = md.indices[i * 3 + 2];

						  // Vertices for this triangle.
						  Vector4 v0 = md.points[i0].pos;
						  Vector4 v1 = md.points[i1].pos;
						  Vector4 v2 = md.points[i2].pos;

						  // We have to iterate over all the triangles in order to find the nearest intersection.
						  float t = 0.0f;
						  if (DirectX::TriangleTests::Intersects(rayOrigin, rayDir, v0,
				                                        v1, v2, t))
						  {
							  if (t < tmin)
							  {
								  // This is the new nearest picked triangle.
								  tmin = t;
								  UINT pickedTriangle = i;

								  Debug::LogMessage(info.name);
							  }
						  }
					  }
				  }
	});
}
