//***************************************************************************************
// Based on GeometryGenerator.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Defines a static class for procedurally generating the geometry of
// common mathematical objects.
//
// All triangles are generated "outward" facing.  If you want "inward"
// facing triangles (for example, if you want to place the camera inside
// a sphere to simulate a sky), you will need to:
//   1. Change the Direct3D cull mode or manually reverse the winding order.
//   2. Invert the normal.
//   3. Update the texture coordinates and tangent vectors.
//***************************************************************************************

#pragma once

#include "assetmng/MeshData.h"

#include <DirectXMath.h>
#include <cstdint>
#include <vector>

namespace D3E
{
	class GeometryGenerator
	{
	public:
		using uint32 = std::uint32_t;

		///< summary>
		/// Creates a box centered at the origin with the given dimensions, where each face has m rows and n columns of vertices.
		///</summary>
		static void CreateBox(MeshData& sm, float width, float height, float depth,
		                   uint32 numSubdivisions);

		///< summary>
		/// Creates a sphere centered at the origin with the given radius.  The
		/// slices and stacks parameters control the degree of tessellation.
		///</summary>
		static void CreateSphere(MeshData& sm, float radius, uint32 sliceCount,
		                      uint32 stackCount);

		///< summary>
		/// Creates a geosphere centered at the origin with the given radius.  The depth controls the level of tessellation.
		///</summary>
		void CreateGeosphere(MeshData& sm, float radius, uint32 numSubdivisions);

		///< summary>
		/// Creates a cylinder parallel to the y-axis, and centered about the origin. The bottom and top radius can vary to form various cone shapes rather than true
		// cylinders.  The slices and stacks parameters control the degree of tessellation.
		///</summary>
		static void CreateCylinder(MeshData& sm, float bottomRadius, float topRadius,
		                        float height, uint32 sliceCount,
		                        uint32 stackCount);

		///< summary>
		/// Creates an mxn grid in the xz-plane with m rows and n columns, centered at the origin with the specified width and depth.
		///</summary>
		static void CreateGrid(MeshData& sm, float width, float depth, uint32 m, uint32 n);

		///< summary>
		/// Creates a quad aligned with the screen.  This is useful for postprocessing and screen effects.
		///</summary>
		static void CreateQuad(MeshData& sm, float x, float y, float w, float h, float depth);

	private:
		static void Subdivide(MeshData& sm);
		static Vertex MidPoint(const Vertex& v0, const Vertex& v1);
		static void BuildCylinderTopCap(MeshData& sm, float bottomRadius,
		                         float topRadius, float height, uint32 sliceCount,
		                         uint32 stackCount);
		static void BuildCylinderBottomCap(MeshData& sm, float bottomRadius,
		                            float topRadius, float height, uint32 sliceCount,
		                            uint32 stackCount);
	};
}
