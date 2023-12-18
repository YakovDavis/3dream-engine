#pragma once

#ifdef JPH_DEBUG_RENDERER
#include "Jolt/Jolt.h"
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Math/Real.h>
#include "Jolt/Core/Color.h"
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

#include <Jolt/Math/DVec3.h>

#include <Jolt/Renderer/DebugRenderer.h>

namespace D3E
{
	class Game;

	class JoltDebugRenderer : public JPH::DebugRenderer
	{
		Game* game_;

	public:
		JoltDebugRenderer(Game* game);

		void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
		void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) override;
		Batch CreateTriangleBatch(const Triangle* inTriangles, int inTriangleCount) override;
		Batch CreateTriangleBatch(const Vertex* inVertices, int inVertexCount, const JPH::uint32* inIndices, int inIndexCount) override;
		void DrawGeometry(JPH::Mat44Arg inModelMatrix, const JPH::AABox& inWorldSpaceBounds, float inLODScaleSq, JPH::ColorArg inModelColor, const GeometryRef& inGeometry, ECullMode inCullMode, ECastShadow inCastShadow, EDrawMode inDrawMode) override;
		void DrawText3D(JPH::Vec3Arg inPosition, const std::string_view& inString, JPH::ColorArg inColor, float inHeight) override;
	};
}
#endif
