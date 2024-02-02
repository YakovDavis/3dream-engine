#pragma once

#include "D3E/CommonHeader.h"
#include "EASTL/fixed_vector.h"
#include "EASTL/queue.h"
#include "SimpleMath.h"
#include "nvrhi/nvrhi.h"

#define MAX_BATCH_VERTEX_COUNT 2048

using namespace DirectX::SimpleMath;

namespace D3E
{
	const String kDebugDrawLinesBindingSetUuid = "8e1e9acb-29b4-42a6-9362-312ade993165";
	const String kDebugDrawPointsBindingSetUuid = "16601f54-1b99-44c5-935d-b0b972986c3a";

	class Game;

	class DebugRenderer
	{
	public:
		struct VertexPosCol
		{
			Vector4 pos;
			Color col;
		};
		struct DebugCB
		{
			VertexPosCol v[MAX_BATCH_VERTEX_COUNT];
		};

		explicit DebugRenderer(nvrhi::IDevice* device, Game* game);

		void QueueAxisAlignedBox(const Vector3& center, const Vector3& extents, const Color& color);

		void DrawLine(const Vector3& from, const Vector3& to, const Color& color);
		void QueueLine(const Vector3& from, const Vector3& to, const Color& color);
		void DrawPoint(const Vector3& location, float size);
		void QueuePoint(const Vector3& location, float size);

		void ProcessQueue();

		void Begin(nvrhi::ICommandList* commandList, nvrhi::IFramebuffer* framebuffer);
		void End();

	private:
		void Flush();

		nvrhi::IDevice* device_;
		nvrhi::ICommandList* commandList_;
		nvrhi::BufferHandle linesCB_;
		nvrhi::BufferHandle linesDebugCB_;
		nvrhi::BufferHandle pointsCB_;
		nvrhi::BufferHandle pointsDebugCB_;
		nvrhi::IFramebuffer* framebuffer_;

		DebugCB linesData_;
		uint16_t linesVertexCount_;
		DebugCB pointsData_;
		uint16_t pointsVertexCount_;

		Game* game_;

		eastl::queue<eastl::fixed_vector<VertexPosCol, 2>> lineDrawQueue_;
		eastl::queue<VertexPosCol> pointsDrawQueue_;
	};
}
