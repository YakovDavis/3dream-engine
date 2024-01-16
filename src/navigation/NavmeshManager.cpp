#include "NavmeshManager.h"

#include "D3E/Components/NavmeshComponent.h"
#include "D3E/Debug.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "EASTL/vector.h"
#include "NavmeshCore.h"
#include "Recast.h"
#include "RecastDump.h"
#include "assetmng/MeshData.h"

#include <cmath>
#include <format>

using namespace D3E;

NavmeshManager::NavmeshManager()
	: cfg_(), heightField_(nullptr), compactHeightField_(nullptr),
	  contourSet_(nullptr), polyMesh_(nullptr), polyMeshDetail_(nullptr),
	  meshData_(nullptr)
{
}

NavmeshManager::NavmeshManager(MeshData* meshData, const NavmeshConfig& cfg)
	: meshData_(meshData), cfg_(cfg), heightField_(nullptr),
	  compactHeightField_(nullptr), contourSet_(nullptr), polyMesh_(nullptr),
	  polyMeshDetail_(nullptr)
{
}

bool NavmeshManager::Build()
{
	Clear();

	if (!meshData_)
	{
		Debug::LogError("[NavmeshManager] : Build(): meshData is null.");

		return false;
	}

	float* bmin = nullptr;
	float* bmax = nullptr;
	eastl::vector<float> vertices;
	eastl::vector<int> triangles;
	const int vertexCount = meshData_->points.size();
	const int triangleCount = meshData_->indices.size() / 3;

	for (auto& v : meshData_->points)
	{
		vertices.push_back(v.pos.x);
		vertices.push_back(v.pos.y);
		vertices.push_back(v.pos.z);
	}

	for (auto i : meshData_->indices)
	{
		triangles.push_back(i);
	}

	rcCalcBounds(vertices.data(), meshData_->points.size(), bmin, bmax);

	rcConfig cfg{};
	cfg.cs = cfg_.cellSize;
	cfg.ch = cfg_.cellHeight;
	cfg.walkableSlopeAngle = cfg_.walkableSlopeAngle;
	cfg.walkableHeight = static_cast<int>(ceilf(cfg_.agentHeight / cfg.ch));
	cfg.walkableClimb = static_cast<int>(floorf(cfg_.agentMaxClimb / cfg.ch));
	cfg.walkableRadius = static_cast<int>(cfg_.agentRadius / cfg.cs);
	cfg.maxEdgeLen = static_cast<int>(cfg_.maxEdgeLen / cfg.cs);
	cfg.maxSimplificationError = cfg_.maxSimplificationError;
	cfg.minRegionArea = static_cast<int>(rcSqr(cfg_.minRegionSize));
	cfg.mergeRegionArea = static_cast<int>(rcSqr(cfg_.regionMergeSize));
	cfg.maxVertsPerPoly = cfg_.maxVertsPerPoly;
	cfg.detailSampleDist =
		cfg_.detailSampleDist < 0.9f ? 0 : cfg.cs * cfg_.detailSampleDist;
	;
	cfg.detailSampleMaxError = cfg.ch * cfg_.detailSampleMaxError;

	rcVcopy(cfg.bmin, bmin);
	rcVcopy(cfg.bmax, bmax);
	rcCalcGridSize(cfg.bmin, cfg.bmax, cfg.cs, &cfg.width, &cfg.height);

	ctx_.resetTimers();

	ctx_.startTimer(RC_TIMER_TOTAL);

	heightField_ = rcAllocHeightfield();

	if (!heightField_)
	{
		Debug::LogError("[NavmeshManager] : Out of memory 'solid'.");

		return false;
	}

	auto r = rcCreateHeightfield(&ctx_, *heightField_, cfg.width, cfg.height,
	                             cfg.bmin, cfg.bmax, cfg.cs, cfg.ch);

	if (!r)
	{
		Debug::LogError(
			"[NavmeshManager] : Could not create solid heightfield.");

		return false;
	}

	triAreas_ = new unsigned char[triangleCount];

	if (!triAreas_)
	{
		Debug::LogError("[NavmeshManager] : Out of memory triAreas_.");

		return false;
	}

	memset(triAreas_, 0, sizeof(unsigned char) * triangleCount);
	rcMarkWalkableTriangles(&ctx_, cfg.walkableSlopeAngle, vertices.data(),
	                        vertexCount, triangles.data(), triangleCount,
	                        triAreas_);

	r = rcRasterizeTriangles(&ctx_, vertices.data(), vertexCount,
	                         triangles.data(), triAreas_, triangleCount,
	                         *heightField_, cfg.walkableClimb);

	if (!r)
	{
		Debug::LogError("[NavmeshManager] : Could not rasterize triangles.");

		return false;
	}

	delete[] triAreas_;
	triAreas_ = nullptr;

	if (cfg_.filterLowHangingObstacles)
		rcFilterLowHangingWalkableObstacles(&ctx_, cfg.walkableClimb,
		                                    *heightField_);
	if (cfg_.filterLedgeSpans)
		rcFilterLedgeSpans(&ctx_, cfg.walkableHeight, cfg.walkableClimb,
		                   *heightField_);
	if (cfg_.filterWalkableLowHeightSpans)
		rcFilterWalkableLowHeightSpans(&ctx_, cfg.walkableHeight,
		                               *heightField_);

	compactHeightField_ = rcAllocCompactHeightfield();
	if (!compactHeightField_)
	{
		Debug::LogError("[NavmeshManager] : Out of memory 'compactHeightField");

		return false;
	}

	r = rcBuildCompactHeightfield(&ctx_, cfg.walkableHeight, cfg.walkableClimb,
	                              *heightField_, *compactHeightField_);
	if (!r)
	{
		Debug::LogError("[NavmeshManager] : Could not build compact data.");

		return false;
	}

	rcFreeHeightField(heightField_);
	heightField_ = nullptr;

	r = rcErodeWalkableArea(&ctx_, cfg.walkableRadius, *compactHeightField_);

	if (!r)
	{
		Debug::LogError("[NavmeshManager] : Could not erode.");
	}

	/*const ConvexVolume* vols = m_geom->getConvexVolumes();
	for (int i = 0; i < m_geom->getConvexVolumeCount(); ++i)
	    rcMarkConvexPolyArea(m_ctx, vols[i].verts, vols[i].nverts, vols[i].hmin,
	                         vols[i].hmax, (unsigned char)vols[i].area,
	*m_chf);*/

	switch (cfg_.partitionType)
	{
		case PartitionType::kLayers:
		{
			auto r = rcBuildLayerRegions(&ctx_, *compactHeightField_, 0,
			                             cfg.minRegionArea);
			if (!r)
			{
				Debug::LogError(
					"[NavmeshManager] : Could not build layer regions.");

				return false;
			}
		}
		case PartitionType::kWatershed:
		{
			auto r = rcBuildDistanceField(&ctx_, *compactHeightField_);

			if (!r)
			{
				Debug::LogError(
					"[NavmeshManager] : Could not build distance field.");

				return false;
			}

			r = rcBuildRegions(&ctx_, *compactHeightField_, 0,
			                   cfg.minRegionArea, cfg.mergeRegionArea);
			if (!r)
			{
				Debug::LogError(
					"[NavmeshManager] : Could not build watershed regions.");

				return false;
			}
		}
		case PartitionType::kMonotone:
		{
			auto r =
				rcBuildRegionsMonotone(&ctx_, *compactHeightField_, 0,
			                           cfg.minRegionArea, cfg.mergeRegionArea);

			if (!r)
			{
				Debug::LogError(
					"buildNavigation: Could not build monotone regions.");

				return false;
			}
		}
		default:
		{
			Debug::LogError("[NavmeshManager] : Unknown partition type");
		}
	}

	contourSet_ = rcAllocContourSet();
	if (!contourSet_)
	{
		Debug::LogError("[NavmeshManager] : Out of memory 'contourSet_'.");
	}

	r = rcBuildContours(&ctx_, *compactHeightField_, cfg.maxSimplificationError,
	                    cfg.maxEdgeLen, *contourSet_);

	if (!r)
	{
		Debug::LogError("[NavmeshManager] : Could not create contours.");

		return false;
	}

	polyMesh_ = rcAllocPolyMesh();
	if (!polyMesh_)
	{
		Debug::LogError("[NavmeshManager] : Out of memory 'polyMesh_'.");

		return false;
	}

	r = rcBuildPolyMesh(&ctx_, *contourSet_, cfg.maxVertsPerPoly, *polyMesh_);

	if (!r)
	{
		Debug::LogError("[NavmeshManager] : Could not triangulate contours.");

		return false;
	}

	polyMeshDetail_ = rcAllocPolyMeshDetail();

	if (!polyMeshDetail_)
	{
		Debug::LogError("[NavmeshManager] : Out of memory 'polyMeshDetail_'.");

		return false;
	}

	r = rcBuildPolyMeshDetail(&ctx_, *polyMesh_, *compactHeightField_,
	                          cfg.detailSampleDist, cfg.detailSampleMaxError,
	                          *polyMeshDetail_);

	if (!r)
	{
		Debug::LogError("[NavmeshManager] : Could not build detail mesh.");

		return false;
	}

	rcFreeCompactHeightfield(compactHeightField_);
	rcFreeContourSet(contourSet_);
	compactHeightField_ = nullptr;
	contourSet_ = nullptr;

	if (cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
	{
		unsigned char* navData = 0;
		int navDataSize = 0;

		for (int i = 0; i < polyMesh_->npolys; ++i)
		{
			if (polyMesh_->areas[i] == RC_WALKABLE_AREA)
				polyMesh_->areas[i] =
					static_cast<unsigned char>(PolyArea::kGround);

			if (polyMesh_->areas[i] ==
			    static_cast<unsigned char>(PolyArea::kGround))
			{
				polyMesh_->flags[i] =
					static_cast<unsigned short>(PolyFlag::kWalk);
			}
			else if (polyMesh_->areas[i] ==
			         static_cast<unsigned char>(PolyArea::kWater))
			{
				polyMesh_->flags[i] =
					static_cast<unsigned short>(PolyFlag::kSwim);
			}
			if (polyMesh_->areas[i] ==
			    static_cast<unsigned char>(PolyArea::kDoor))
			{
				polyMesh_->flags[i] =
					static_cast<unsigned short>(PolyFlag::kWalk) |
					static_cast<unsigned short>(PolyFlag::kDoor);
			}
		}

		dtNavMeshCreateParams params;
		memset(&params, 0, sizeof(params));
		params.verts = polyMesh_->verts;
		params.vertCount = polyMesh_->nverts;
		params.polys = polyMesh_->polys;
		params.polyAreas = polyMesh_->areas;
		params.polyFlags = polyMesh_->flags;
		params.polyCount = polyMesh_->npolys;
		params.nvp = polyMesh_->nvp;
		params.detailMeshes = polyMeshDetail_->meshes;
		params.detailVerts = polyMeshDetail_->verts;
		params.detailVertsCount = polyMeshDetail_->nverts;
		params.detailTris = polyMeshDetail_->tris;
		params.detailTriCount = polyMeshDetail_->ntris;
		params.walkableHeight = cfg_.agentHeight;
		params.walkableRadius = cfg_.agentRadius;
		params.walkableClimb = cfg_.agentMaxClimb;
		rcVcopy(params.bmin, polyMesh_->bmin);
		rcVcopy(params.bmax, polyMesh_->bmax);
		params.cs = cfg.cs;
		params.ch = cfg.ch;
		params.buildBvTree = true;

		auto r = dtCreateNavMeshData(&params, &navData, &navDataSize);
		if (!r)
		{
			Debug::LogError(
				"[NavmeshManager] : Could not build Detour navmesh.");

			return false;
		}

		navMesh_ = dtAllocNavMesh();
		if (!navMesh_)
		{
			dtFree(navData);
			Debug::LogError(
				"[NavmeshManager] : Could not create Detour navmesh");

			return false;
		}

		auto status = navMesh_->init(navData, navDataSize, DT_TILE_FREE_DATA);
		if (dtStatusFailed(status))
		{
			dtFree(navData);
			Debug::LogError("[NavmeshManager] : Could not init Detour navmesh");

			return false;
		}

		status = navQuery_->init(navMesh_, 2048);
		if (dtStatusFailed(status))
		{
			Debug::LogError(
				"[NavmeshManager] : Could not init Detour navmesh query");

			return false;
		}
	}

	ctx_.stopTimer(RC_TIMER_TOTAL);

	duLogBuildTimes(ctx_, ctx_.getAccumulatedTime(RC_TIMER_TOTAL));
	Debug::LogMessage(std::format("Polymesh: %d vertices  %d polygons",
	                              polyMesh_->nverts, polyMesh_->npolys)
	                      .c_str());

	return true;
}

void NavmeshManager::Clear()
{
	cfg_ = {};

	rcFreeHeightField(heightField_);
	rcFreeCompactHeightfield(compactHeightField_);
	rcFreeContourSet(contourSet_);
	rcFreePolyMesh(polyMesh_);
	rcFreePolyMeshDetail(polyMeshDetail_);
	dtFreeNavMesh(navMesh_);

	heightField_ = nullptr;
	compactHeightField_ = nullptr;
	contourSet_ = nullptr;
	polyMesh_ = nullptr;
	polyMeshDetail_ = nullptr;
	navMesh_ = nullptr;
	meshData_ = nullptr;
}
