#include "DebugRenderer.h"

#include "CameraUtils.h"
#include "D3E/Components/TransformComponent.h"
#include "D3E/Components/render/CameraComponent.h"
#include "D3E/Debug.h"
#include "D3E/Game.h"
#include "ShaderFactory.h"
#include "core/EngineState.h"
#include "render/PerObjectConstBuffer.h"

void D3E::DebugRenderer::DrawLine(const Vector3& from, const Vector3& to,
                                  const Color& color)
{
	if (linesVertexCount_ > MAX_BATCH_VERTEX_COUNT - 2)
	{
		Flush();
	}

	VertexPosCol v1, v2;

	v1.pos = Vector4(from.x, from.y, from.z, 1.0f);
	v1.col = color;
	v2.pos = Vector4(to.x, to.y, to.z, 1.0f);
	v2.col = color;

	linesData_.v[linesVertexCount_] = v1;
	linesData_.v[linesVertexCount_ + 1] = v2;

	linesVertexCount_ += 2;
}

void D3E::DebugRenderer::DrawPoint(const Vector3& location, float size)
{
	// TODO: implement
}

void D3E::DebugRenderer::Begin(nvrhi::ICommandList* commandList, nvrhi::IFramebuffer* framebuffer)
{
	commandList_ = commandList;
	framebuffer_ = framebuffer;
	linesVertexCount_ = 0;
	pointsVertexCount_ = 0;
}

void D3E::DebugRenderer::End()
{
	Flush();
}

D3E::DebugRenderer::DebugRenderer(nvrhi::IDevice* device, Game* game) : device_(device), commandList_(nullptr), game_(game)
{
	nvrhi::BufferDesc desc1 = {};
	desc1.setIsConstantBuffer(true);
	desc1.setByteSize(sizeof(PerObjectConstBuffer));
	desc1.setIsVolatile(false);
	desc1.setMaxVersions(16);
	desc1.setKeepInitialState(true);
	linesCB_ = device_->createBuffer(desc1);
	pointsCB_ = device_->createBuffer(desc1);

	nvrhi::BufferDesc desc2 = {};
	desc2.setIsConstantBuffer(true);
	desc2.setByteSize(sizeof(DebugCB));
	desc2.setIsVolatile(false);
	desc2.setMaxVersions(16);
	desc2.setKeepInitialState(true);
	linesDebugCB_ = device_->createBuffer(desc2);
	pointsDebugCB_ = device_->createBuffer(desc2);

	nvrhi::BindingSetDesc linesBindingSetDescV = {};
	linesBindingSetDescV.addItem(nvrhi::BindingSetItem::ConstantBuffer(0, linesCB_));
	linesBindingSetDescV.addItem(nvrhi::BindingSetItem::ConstantBuffer(1, linesDebugCB_));
	ShaderFactory::AddBindingSetV(kDebugDrawLinesBindingSetUuid, linesBindingSetDescV, "DebugDrawV");
	nvrhi::BindingSetDesc linesBindingSetDescP = {};
	ShaderFactory::AddBindingSetP(kDebugDrawLinesBindingSetUuid, linesBindingSetDescP, "DebugDrawP");

	nvrhi::BindingSetDesc pointsBindingSetDescV = {};
	pointsBindingSetDescV.addItem(nvrhi::BindingSetItem::ConstantBuffer(0, pointsCB_));
	pointsBindingSetDescV.addItem(nvrhi::BindingSetItem::ConstantBuffer(1, pointsDebugCB_));
	ShaderFactory::AddBindingSetV(kDebugDrawPointsBindingSetUuid, pointsBindingSetDescV, "DebugDrawV");
	nvrhi::BindingSetDesc pointsBindingSetDescP = {};
	ShaderFactory::AddBindingSetP(kDebugDrawPointsBindingSetUuid, pointsBindingSetDescP, "DebugDrawP");
}

void D3E::DebugRenderer::Flush()
{
	if (linesVertexCount_ == 0 && pointsVertexCount_ > 0)
	{
		return;
	}

	Vector3 origin = {0, 0, 0};

	auto playerView = game_->GetRegistry().view<const TransformComponent, const CameraComponent>();

	const CameraComponent* camera = nullptr;

	for(auto [entity, tc, cc] : playerView.each())
	{
		origin = tc.position + cc.offset;
		camera = &cc;
		break;
	}

	if (!camera)
	{
		Debug::LogWarning("[DebugRenderer] Camera not found");
		return;
	}

	PerObjectConstBuffer constBufferData1 = {};
	const DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::Identity;
	constBufferData1.gWorldViewProj = world * CameraUtils::GetViewProj(origin, *camera);
	constBufferData1.gWorld = world;
	constBufferData1.gWorldView = world * CameraUtils::GetView(origin, *camera);
	constBufferData1.gInvTrRotation = world.Invert().Transpose();
	constBufferData1.gEditorId = 0;

	if (linesVertexCount_ > 0)
	{
		commandList_->writeBuffer(linesCB_, &constBufferData1, sizeof(constBufferData1));

		commandList_->writeBuffer(linesDebugCB_, &linesData_, linesVertexCount_ * sizeof(VertexPosCol));

		auto graphicsState = nvrhi::GraphicsState()
		                         .setPipeline(ShaderFactory::GetGraphicsPipeline("DebugDraw"))
		                         .setFramebuffer(framebuffer_)
		                         .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(EngineState::GetViewportWidth(), EngineState::GetViewportHeight())))
		                         .addBindingSet(ShaderFactory::GetBindingSetV(kDebugDrawLinesBindingSetUuid))
		                         .addBindingSet(ShaderFactory::GetBindingSetP(kDebugDrawLinesBindingSetUuid));
		commandList_->setGraphicsState(graphicsState);

		nvrhi::DrawArguments args = {};
		args.setVertexCount(linesVertexCount_);
		commandList_->draw(args);
	}
	if (pointsVertexCount_ > 0)
	{
		commandList_->writeBuffer(pointsCB_, &constBufferData1, sizeof(constBufferData1));

		commandList_->writeBuffer(pointsDebugCB_, &pointsData_, pointsVertexCount_ * sizeof(VertexPosCol));

		auto graphicsState = nvrhi::GraphicsState()
		                         .setPipeline(ShaderFactory::GetGraphicsPipeline("DebugDraw"))
		                         .setFramebuffer(framebuffer_)
		                         .setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(1280, 720))) // TODO
		                         .addBindingSet(ShaderFactory::GetBindingSetV(kDebugDrawPointsBindingSetUuid))
		                         .addBindingSet(ShaderFactory::GetBindingSetP(kDebugDrawPointsBindingSetUuid));
		commandList_->setGraphicsState(graphicsState);

		nvrhi::DrawArguments args = {};
		args.setVertexCount(pointsVertexCount_);
		commandList_->draw(args);
	}
	linesVertexCount_ = 0;
	pointsVertexCount_ = 0;
}

void D3E::DebugRenderer::QueueLine(const Vector3& from, const Vector3& to,
                                   const Color& color)
{
	VertexPosCol v1, v2;

	v1.pos = Vector4(from.x, from.y, from.z, 1.0f);
	v1.col = color;
	v2.pos = Vector4(to.x, to.y, to.z, 1.0f);
	v2.col = color;

	lineDrawQueue_.push(v1);
	lineDrawQueue_.push(v2);
}

void D3E::DebugRenderer::QueuePoint(const Vector3& location, float size)
{
	// TODO: implement
}

void D3E::DebugRenderer::ProcessQueue()
{
	Flush();

	while (lineDrawQueue_.size() > MAX_BATCH_VERTEX_COUNT)
	{
		for (uint16_t i = 0; i < MAX_BATCH_VERTEX_COUNT; ++i)
		{
			linesData_.v[i] = lineDrawQueue_.front();
			lineDrawQueue_.pop();
			linesVertexCount_ = i + 1;
		}
		Flush();
	}

	for (uint16_t i = 0; i < lineDrawQueue_.size(); ++i)
	{
		linesData_.v[i] = lineDrawQueue_.front();
		lineDrawQueue_.pop();
		linesVertexCount_ = i + 1;
	}
	Flush();

	while (pointsDrawQueue_.size() > MAX_BATCH_VERTEX_COUNT)
	{
		for (uint16_t i = 0; i < MAX_BATCH_VERTEX_COUNT; ++i)
		{
			pointsData_.v[i] = pointsDrawQueue_.front();
			pointsDrawQueue_.pop();
			pointsVertexCount_ = i + 1;
		}
		Flush();
	}

	for (uint16_t i = 0; i < pointsDrawQueue_.size(); ++i)
	{
		pointsData_.v[i] = pointsDrawQueue_.front();
		pointsDrawQueue_.pop();
		pointsVertexCount_ = i + 1;
	}
	Flush();
}
