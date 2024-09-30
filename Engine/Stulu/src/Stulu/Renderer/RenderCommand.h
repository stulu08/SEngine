#pragma once
#include "Stulu/Renderer/RenderAPI.h"

namespace Stulu{
	class STULU_API RenderCommand {
	public:
		static void init();

		inline static void setDefault() {
			s_renderAPI->setDefault();
		}
		inline static void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
			s_renderAPI->setViewport(x, y, width, height);
		}
		inline static void setWireFrame(bool value) {
			s_renderAPI->setWireFrame(value);
		}
		inline static void setCullMode(CullMode value) {
			s_renderAPI->setCullMode(value);
		}
		inline static void setStencil(StencilMode value) {
			s_renderAPI->setStencil(value);
		}
		inline static void setDepthTesting(bool value) {
			s_renderAPI->setDepthTesting(value);
		}
		inline static void setClearColor(const glm::vec4& color) {
			s_renderAPI->setClearColor(color);
		}
		inline static void clear() {
			s_renderAPI->clear();
		}
		inline static void drawIndexed(const Ref<VertexArray>& vertexArray, const uint32_t count = 0, const uint32_t instanceCount = 0) {
#if ST_PROFILING_RENDERDATA
			ST_PROFILING_RENDERDATA_ADDDRAWCALLS(1);
			for (auto& vb : vertexArray->getVertexBuffers()) {
				ST_PROFILING_RENDERDATA_ADDVERTICES(vb->getCount());
			}
			ST_PROFILING_RENDERDATA_ADDINDICES(count == 0 ? vertexArray->getIndexBuffer()->getCount() : count);
#endif
			s_renderAPI->drawIndexed(vertexArray, count, instanceCount);
		}
		inline static void drawLines(const Ref<VertexArray>& vertexArray, const uint32_t count = 0) {
#if ST_PROFILING_RENDERDATA
			ST_PROFILING_RENDERDATA_ADDDRAWCALLS(1);
			for (auto& vb : vertexArray->getVertexBuffers()) {
				ST_PROFILING_RENDERDATA_ADDVERTICES(vb->getCount());
			}
			ST_PROFILING_RENDERDATA_ADDINDICES(count);
#endif
			s_renderAPI->drawLines(vertexArray, count);
		}
		struct Limits {
			inline static glm::ivec3 getMaxComputeWorkGroupCount() {
				return s_renderAPI->getMaxComputeWorkGroupCount();
			}
			inline static glm::ivec3 getMaxComputeWorkGroupSizes() {
				return s_renderAPI->getMaxComputeWorkGroupSizes();
			}
			inline static uint32_t getMaxComputeWorkGroupInvocationCount() {
				return s_renderAPI->getMaxComputeWorkGroupInvocationCount();
			}
		};

		static inline RenderAPI* GetApi() {
			return s_renderAPI.get();
		}
	private:
		static Scope<RenderAPI> s_renderAPI;
	};
}