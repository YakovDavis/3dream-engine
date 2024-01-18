#include "NavmeshBuilder.h"

#include "D3E/Components/navigation/NavmeshComponent.h"
#include "D3E/Components/navigation/NavmeshCore.h"
#include "D3E/Debug.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "EASTL/vector.h"
#include "Recast.h"
#include "RecastDump.h"
#include "assetmng/MeshData.h"

#include <cmath>
#include <format>

using namespace D3E;
using namespace eastl;

NavmeshBuilder::NavmeshBuilder()
	: cfg_(), heightField_(nullptr), compactHeightField_(nullptr),
	  contourSet_(nullptr), polyMesh_(nullptr), polyMeshDetail_(nullptr),
	  crowd_(nullptr), navMesh_(nullptr), navQuery_(nullptr),
	  triAreas_(nullptr), vertices_(nullptr), indices_(nullptr)
{
	navQuery_ = dtAllocNavMeshQuery();
	crowd_ = dtAllocCrowd();
}

NavmeshBuilder::NavmeshBuilder(const vector<float>* v, const vector<int>* i)
	: cfg_(), heightField_(nullptr), compactHeightField_(nullptr),
	  contourSet_(nullptr), polyMesh_(nullptr), polyMeshDetail_(nullptr),
	  crowd_(nullptr), navMesh_(nullptr), navQuery_(nullptr),
	  triAreas_(nullptr), vertices_(v), indices_(i)
{
	navQuery_ = dtAllocNavMeshQuery();
	crowd_ = dtAllocCrowd();
}

NavmeshBuilder::~NavmeshBuilder()
{
	Clear();
}

bool NavmeshBuilder::Build(NavmeshComponent& nc)
{
	Clear();

	cfg_ = nc.config;

	if (!vertices_ || !indices_)
	{
		Debug::LogError(
			"[NavmeshBuilder] : Build(): vertices or indices is nullptr.");

		return false;
	}

	const int vertexCount = vertices_->size() / 3;
	const int triangleCount = indices_->size() / 3;

	eastl::vector<float> bmin(3);
	eastl::vector<float> bmax(3);

	rcCalcBounds(vertices_->data(), vertexCount, bmin.data(), bmax.data());

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

	rcVcopy(cfg.bmin, bmin.data());
	rcVcopy(cfg.bmax, bmax.data());
	rcCalcGridSize(cfg.bmin, cfg.bmax, cfg.cs, &cfg.width, &cfg.height);

	ctx_.resetTimers();

	ctx_.startTimer(RC_TIMER_TOTAL);

	heightField_ = rcAllocHeightfield();

	if (!heightField_)
	{
		Debug::LogError("[NavmeshBuilder] : Out of memory 'solid'.");

		return false;
	}

	auto r = rcCreateHeightfield(&ctx_, *heightField_, cfg.width, cfg.height,
	                             cfg.bmin, cfg.bmax, cfg.cs, cfg.ch);

	if (!r)
	{
		Debug::LogError(
			"[NavmeshBuilder] : Could not create solid heightfield.");

		return false;
	}

	triAreas_ = new unsigned char[triangleCount];

	if (!triAreas_)
	{
		Debug::LogError("[NavmeshBuilder] : Out of memory triAreas_.");

		return false;
	}

	memset(triAreas_, 0, sizeof(unsigned char) * triangleCount);
	rcMarkWalkableTriangles(&ctx_, cfg.walkableSlopeAngle, vertices_->data(),
	                        vertexCount, indices_->data(), triangleCount,
	                        triAreas_);

	r = rcRasterizeTriangles(&ctx_, vertices_->data(), vertexCount,
	                         indices_->data(), triAreas_, triangleCount,
	                         *heightField_, cfg.walkableClimb);

	if (!r)
	{
		Debug::LogError("[NavmeshBuilder] : Could not rasterize triangles.");

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
		Debug::LogError("[NavmeshBuilder] : Out of memory 'compactHeightField");

		return false;
	}

	r = rcBuildCompactHeightfield(&ctx_, cfg.walkableHeight, cfg.walkableClimb,
	                              *heightField_, *compactHeightField_);
	if (!r)
	{
		Debug::LogError("[NavmeshBuilder] : Could not build compact data.");

		return false;
	}

	rcFreeHeightField(heightField_);
	heightField_ = nullptr;

	r = rcErodeWalkableArea(&ctx_, cfg.walkableRadius, *compactHeightField_);

	if (!r)
	{
		Debug::LogError("[NavmeshBuilder] : Could not erode.");
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
					"[NavmeshBuilder] : Could not build layer regions.");

				return false;
			}

			break;
		}
		case PartitionType::kWatershed:
		{
			auto r = rcBuildDistanceField(&ctx_, *compactHeightField_);

			if (!r)
			{
				Debug::LogError(
					"[NavmeshBuilder] : Could not build distance field.");

				return false;
			}

			r = rcBuildRegions(&ctx_, *compactHeightField_, 0,
			                   cfg.minRegionArea, cfg.mergeRegionArea);
			if (!r)
			{
				Debug::LogError(
					"[NavmeshBuilder] : Could not build watershed regions.");

				return false;
			}

			break;
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

			break;
		}
		default:
		{
			Debug::LogError("[NavmeshBuilder] : Unknown partition type");
		}
	}

	contourSet_ = rcAllocContourSet();
	if (!contourSet_)
	{
		Debug::LogError("[NavmeshBuilder] : Out of memory 'contourSet_'.");

		return false;
	}

	r = rcBuildContours(&ctx_, *compactHeightField_, cfg.maxSimplificationError,
	                    cfg.maxEdgeLen, *contourSet_);

	if (!r)
	{
		Debug::LogError("[NavmeshBuilder] : Could not create contours.");

		return false;
	}

	polyMesh_ = rcAllocPolyMesh();
	if (!polyMesh_)
	{
		Debug::LogError("[NavmeshBuilder] : Out of memory 'polyMesh_'.");

		return false;
	}

	r = rcBuildPolyMesh(&ctx_, *contourSet_, cfg.maxVertsPerPoly, *polyMesh_);

	if (!r)
	{
		Debug::LogError("[NavmeshBuilder] : Could not triangulate contours.");

		return false;
	}

	polyMeshDetail_ = rcAllocPolyMeshDetail();

	if (!polyMeshDetail_)
	{
		Debug::LogError("[NavmeshBuilder] : Out of memory 'polyMeshDetail_'.");

		return false;
	}

	r = rcBuildPolyMeshDetail(&ctx_, *polyMesh_, *compactHeightField_,
	                          cfg.detailSampleDist, cfg.detailSampleMaxError,
	                          *polyMeshDetail_);

	if (!r)
	{
		Debug::LogError("[NavmeshBuilder] : Could not build detail mesh.");

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
				"[NavmeshBuilder] : Could not build Detour navmesh.");

			return false;
		}

		navMesh_ = dtAllocNavMesh();
		if (!navMesh_)
		{
			dtFree(navData);
			Debug::LogError(
				"[NavmeshBuilder] : Could not create Detour navmesh");

			return false;
		}

		auto status = navMesh_->init(navData, navDataSize, DT_TILE_FREE_DATA);
		if (dtStatusFailed(status))
		{
			dtFree(navData);
			Debug::LogError("[NavmeshBuilder] : Could not init Detour navmesh");

			return false;
		}

		status = navQuery_->init(navMesh_, 2048);
		if (dtStatusFailed(status))
		{
			Debug::LogError(
				"[NavmeshBuilder] : Could not init Detour navmesh query");

			return false;
		}
	}

	ctx_.stopTimer(RC_TIMER_TOTAL);

	duLogBuildTimes(ctx_, ctx_.getAccumulatedTime(RC_TIMER_TOTAL));
	Debug::LogMessage(std::format("Polymesh: %d vertices  %d polygons",
	                              polyMesh_->nverts, polyMesh_->npolys)
	                      .c_str());

	nc.isBuilt = true;
	nc.navMesh = navMesh_;
	nc.navQuery = navQuery_;
	nc.crowd = crowd_;

	return true;
}

void NavmeshBuilder::Clear()
{
	rcFreeHeightField(heightField_);
	rcFreeCompactHeightfield(compactHeightField_);
	rcFreeContourSet(contourSet_);
	rcFreePolyMesh(polyMesh_);
	rcFreePolyMeshDetail(polyMeshDetail_);

	heightField_ = nullptr;
	compactHeightField_ = nullptr;
	contourSet_ = nullptr;
	polyMesh_ = nullptr;
	polyMeshDetail_ = nullptr;
}
