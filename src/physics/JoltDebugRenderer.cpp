#include "JoltDebugRenderer.h"
#include "D3E/Game.h"
#include "render/GameRender.h"
#include "render/DebugRenderer.h"

#ifdef JPH_DEBUG_RENDERER

using Batch = JPH::Ref<JPH::RefTargetVirtual>;

class BatchImpl : public JPH::RefTargetVirtual
{
public:
	eastl::vector<DirectX::SimpleMath::Vector3> triangles;
	DirectX::SimpleMath::Color BatchColor;

	int mRefCount = 0;

	void AddRef() override { mRefCount++; }
	void Release() override { if (--mRefCount == 0) delete this; }
};

D3E::JoltDebugRenderer::JoltDebugRenderer(Game* game) : game_(game)
{
	Initialize();
}

void D3E::JoltDebugRenderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo,
                                      JPH::ColorArg inColor)
{
	game_->GetRender()->GetDebugRenderer()->QueueLine(DirectX::SimpleMath::Vector3(inFrom.GetX(), inFrom.GetY(), inFrom.GetZ()), DirectX::SimpleMath::Vector3(inTo.GetX(), inTo.GetY(), inTo.GetZ()), DirectX::SimpleMath::Color(inColor.r, inColor.g, inColor.b));
}

void D3E::JoltDebugRenderer::DrawTriangle(JPH::Vec3Arg inV1, JPH::Vec3Arg inV2,
                                          JPH::Vec3Arg inV3,
                                          JPH::ColorArg inColor,
                                          ECastShadow inCastShadow)
{
	JPH::Float3 p0, p1, p2;
	inV1.StoreFloat3(&p0);
	inV2.StoreFloat3(&p1);
	inV3.StoreFloat3(&p2);

	DrawLine(JPH::RVec3Arg(p0), JPH::RVec3Arg(p1), inColor);
	DrawLine(JPH::RVec3Arg(p0), JPH::RVec3Arg(p2), inColor);
	DrawLine(JPH::RVec3Arg(p1), JPH::RVec3Arg(p2), inColor);
}

Batch D3E::JoltDebugRenderer::CreateTriangleBatch(const Triangle* inTriangles,
                                                  int inTriangleCount)
{
	auto batch = new BatchImpl();

	for (int i = 0; i < inTriangleCount; ++i)
	{
		auto& tri = inTriangles[i];

		auto p0 = tri.mV[0].mPosition;
		auto p1 = tri.mV[1].mPosition;
		auto p2 = tri.mV[2].mPosition;

		batch->triangles.push_back(DirectX::SimpleMath::Vector3(p0.x, p0.y, p0.z));
		batch->triangles.push_back(DirectX::SimpleMath::Vector3(p1.x, p1.y, p1.z));
		batch->triangles.push_back(DirectX::SimpleMath::Vector3(p2.x, p2.y, p2.z));
	}

	if (inTriangleCount > 0) {
		auto color = inTriangles[0].mV[0].mColor;
		batch->BatchColor = DirectX::SimpleMath::Color(color.r, color.g, color.b);
	}

	return batch;
}

Batch D3E::JoltDebugRenderer::CreateTriangleBatch(const Vertex* inVertices,
                                                  int inVertexCount,
                                                  const JPH::uint32* inIndices,
                                                  int inIndexCount)
{
	auto batch = new BatchImpl();

	for (int i = 0; i < inIndexCount; i += 3)
	{
		auto p0 = inVertices[inIndices[i+0]].mPosition;
		auto p1 = inVertices[inIndices[i+1]].mPosition;
		auto p2 = inVertices[inIndices[i+2]].mPosition;

		batch->triangles.push_back(DirectX::SimpleMath::Vector3(p0.x, p0.y, p0.z));
		batch->triangles.push_back(DirectX::SimpleMath::Vector3(p1.x, p1.y, p1.z));
		batch->triangles.push_back(DirectX::SimpleMath::Vector3(p2.x, p2.y, p2.z));
	}

	if (inVertexCount > 0) {
		auto color = inVertices[0].mColor;
		batch->BatchColor = DirectX::SimpleMath::Color(color.r, color.g, color.b);
	}

	return batch;
}

void D3E::JoltDebugRenderer::DrawGeometry(
	JPH::Mat44Arg inModelMatrix, const JPH::AABox& inWorldSpaceBounds,
	float inLODScaleSq, JPH::ColorArg inModelColor,
	const GeometryRef& inGeometry, ECullMode inCullMode,
	ECastShadow inCastShadow, EDrawMode inDrawMode)
{
	auto col0 = inModelMatrix.GetColumn4(0);
	auto col1 = inModelMatrix.GetColumn4(1);
	auto col2 = inModelMatrix.GetColumn4(2);
	auto col3 = inModelMatrix.GetColumn4(3);

	DirectX::SimpleMath::Matrix mat(
		col0.GetX(), col0.GetY(), col0.GetZ(), col0.GetW(),
		col1.GetX(), col1.GetY(), col1.GetZ(), col1.GetW(),
		col2.GetX(), col2.GetY(), col2.GetZ(), col2.GetW(),
		col3.GetX(), col3.GetY(), col3.GetZ(), col3.GetW());


	auto pos = inModelMatrix.GetTranslation();
	game_->GetRender()->GetDebugRenderer()->QueuePoint(DirectX::SimpleMath::Vector3(pos.GetX(), pos.GetY(), pos.GetZ()), 10);

	auto batch = (BatchImpl*)inGeometry->mLODs[0].mTriangleBatch.GetPtr();

	for (int i = 0; i < batch->triangles.size(); i+=3)
	{
		auto p0 = DirectX::SimpleMath::Vector3::Transform(batch->triangles[i + 0], mat);
		auto p1 = DirectX::SimpleMath::Vector3::Transform(batch->triangles[i + 1], mat);
		auto p2 = DirectX::SimpleMath::Vector3::Transform(batch->triangles[i + 2], mat);

		game_->GetRender()->GetDebugRenderer()->QueueLine(p0, p1, batch->BatchColor);
		game_->GetRender()->GetDebugRenderer()->QueueLine(p0, p2, batch->BatchColor);
		game_->GetRender()->GetDebugRenderer()->QueueLine(p1, p2, batch->BatchColor);
	}
}

void D3E::JoltDebugRenderer::DrawText3D(JPH::Vec3Arg inPosition,
                                        const std::string_view& inString,
                                        JPH::ColorArg inColor, float inHeight)
{
}
#endif
