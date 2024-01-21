#pragma once

#include "D3E/CommonHeader.h"
#include "EASTL/memory.h"
#include "EASTL/vector.h"
#include "EASTL/map.h"
#include "EASTL/set.h"
#include <cstdint>
#include "SimpleMath.h"

#include <nvrhi/nvrhi.h>

#include "RmlUi/Core/RenderInterface.h"

namespace D3E
{
	class Game;

	using namespace Rml;

	class RmlUi_NVRHI final : public RenderInterface
	{
	public:
		RmlUi_NVRHI(Game* game, nvrhi::IFramebuffer* fb);
		~RmlUi_NVRHI() override;

		void RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, TextureHandle texture,
		                            const Vector2f& translation) override;

		CompiledGeometryHandle CompileGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, TextureHandle texture) override;

		void RenderCompiledGeometry(CompiledGeometryHandle geometry, const Vector2f& translation) override;

		void ReleaseCompiledGeometry(CompiledGeometryHandle geometry) override;

		void EnableScissorRegion(bool enable) override;

		void SetScissorRegion(int x, int y, int width, int height) override;

		bool LoadTexture(TextureHandle& texture_handle, Vector2i& texture_dimensions, const Rml::String& source) override;

		bool GenerateTexture(TextureHandle& texture_handle, const byte* source, const Vector2i& source_dimensions) override;

		void ReleaseTexture(TextureHandle texture) override;

		void SetTransform(const Matrix4f* transform) override;

	private:
		nvrhi::BindingSetHandle& GetBindingSetHandle(const Rml::TextureHandle& texture);
		bool ReallocateBuffer(nvrhi::BufferHandle& buffer, size_t requiredSize, size_t reallocateSize, bool isIndexBuffer);
		Rml::TextureHandle GetNewTextureHandle();
		Rml::CompiledGeometryHandle GetNewCompiledGeometryHandle();

		struct RmlVertex
		{
			DirectX::SimpleMath::Vector4 pos;
			DirectX::SimpleMath::Vector4 col;
			DirectX::SimpleMath::Vector4 tex;
		};

		struct RmlCB
		{
			DirectX::SimpleMath::Matrix gTransform;
			DirectX::SimpleMath::Vector4 gTranslate;
		};

		struct RmlCompiledGeometry
		{
			int numIndices;
			nvrhi::BufferHandle vertexBuffer;
			nvrhi::BufferHandle indexBuffer;
			nvrhi::VertexBufferBinding vertexBufferBinding;
			nvrhi::IndexBufferBinding indexBufferBinding;
			Rml::TextureHandle texture;
		};

		eastl::map<Rml::CompiledGeometryHandle, RmlCompiledGeometry> compiledGeometryCache_;

		Game* game_;

		RmlCB cbData;

		nvrhi::DeviceHandle renderer_;
		nvrhi::CommandListHandle commandList_;

		nvrhi::BufferHandle constantBuffer_;

		nvrhi::GraphicsPipelineHandle psoCol_;
		nvrhi::GraphicsPipelineHandle psoTex_;

		nvrhi::GraphicsState graphicsState_;
		nvrhi::ViewportState viewportState_;
		nvrhi::Rect scissorsRect_;

		nvrhi::SamplerHandle sampler_;
		nvrhi::BindingLayoutHandle bindingLayout_;

		eastl::map<Rml::TextureHandle, nvrhi::TextureHandle> textureMap_;
		eastl::set<Rml::TextureHandle> takenTextureHandles_;
		eastl::map<Rml::TextureHandle, D3E::String> textureUuids_;
		eastl::map<Rml::TextureHandle, nvrhi::BindingSetHandle> bindingsCache_;

		eastl::set<Rml::CompiledGeometryHandle> takenCompiledGeometryHandles_;

		nvrhi::BindingSetHandle nullBindingSetP_;
		nvrhi::BindingSetHandle bindingSetV_;
	};
}
